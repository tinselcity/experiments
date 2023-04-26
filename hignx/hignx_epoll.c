//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "ndebug.h"
#include "ext/http-parser/http_parser.h"
#include "mime.h"
#include "date.h"
//! ----------------------------------------------------------------------------
//! constants
//! ----------------------------------------------------------------------------
#define _DEFAULT_PORT 12345
#define _MAX_PENDING 16384
#define _NUM_EVENTS 512
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
typedef struct {
  int fd;
  struct sockaddr_in sa;
  http_parser hp;
  http_parser_settings hps;
  char path[256];
  size_t path_len;
  char resp[1024];
  size_t resp_len;
  off_t resp_off;
  int file_fd;
  size_t file_size;
  off_t file_off;
} conn_t;
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _hp_on_url(http_parser *parser, const char *at, size_t len)
{
  conn_t *conn = (conn_t*)(parser->data);
  if (!conn) {
    return STATUS_ERROR;
  }
  conn->path_len = len > sizeof(conn->path) ? sizeof(conn->path)-1 : len;
  strncpy(conn->path, at, conn->path_len);
  return STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static void _shutdown_conn(conn_t* conn, int epoll) {
  if (epoll > 0) {
    epoll_ctl(epoll, EPOLL_CTL_DEL, conn->fd, NULL);
  }
  if (conn->fd != -1) {
    close(conn->fd);
  }
  if (conn->file_fd != -1) {
    close(conn->file_fd);
  }
  free(conn);
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static void _reset_conn(conn_t* conn) {
  conn->fd = -1;
  conn->hps.on_url = _hp_on_url;
  http_parser_init(&(conn->hp), HTTP_REQUEST);
  conn->hp.data = conn;
  conn->resp_len = 0;
  conn->resp_off = 0;
  conn->file_fd = -1;
  conn->file_size = 0;
  conn->file_off = 0;
}
//! ----------------------------------------------------------------------------
//! \details: Print the command line help.
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _accept_conn(int fd, int epoll) {
  // -------------------------------------------------------
  // conn setup
  // -------------------------------------------------------
  conn_t* conn = (conn_t*)calloc(sizeof(conn_t), 1);
  _reset_conn(conn);
  // -------------------------------------------------------
  // accept
  // -------------------------------------------------------
  socklen_t slen = sizeof(conn->sa);
  conn->fd = accept4(fd,
    (struct sockaddr *) &(conn->sa),
    &slen,
    SOCK_NONBLOCK|SOCK_CLOEXEC);
  if (conn->fd < 0) {
    _shutdown_conn(conn, -1);
    FATAL("error accept() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  //TRACE("[%d] connect from %s:%d\n",
  //  conn->fd,
  //  inet_ntoa(conn->sa.sin_addr),
  //  ntohs(conn->sa.sin_port));
  // -------------------------------------------------------
  // make non-blocking
  // -------------------------------------------------------
  int ioval = 1;
  if (ioctl(conn->fd, FIONBIO, &ioval) < 0) {
    _shutdown_conn(conn, -1);
    FATAL("error fcntl() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  // -------------------------------------------------------
  // register with epoll
  // -------------------------------------------------------
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.ptr = conn;
  if (epoll_ctl(epoll, EPOLL_CTL_ADD, conn->fd, &ev) < 0) {
    _shutdown_conn(conn, -1);
    FATAL("error epoll_ctl() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  return STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: Print the command line help.
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void print_usage(FILE* stream, int exit_code) {
  fprintf(stream, "Usage: tee_splice [options]\n");
  fprintf(stream, "Options:\n");
  fprintf(stream, "  -h, --help  display this help and exit.\n");
  fprintf(stream, "  -p, --port  port (default %d)\n", _DEFAULT_PORT);
  exit(exit_code);
}
//! ----------------------------------------------------------------------------
//! \details main
//! \return  0 on success
//!          -1 on error
//! \param   argc/argv...
//! ----------------------------------------------------------------------------
int main(int argc, char** argv) {
  uint16_t port = _DEFAULT_PORT;
  // -------------------------------------------------------
  // arg defs
  // -------------------------------------------------------
  char opt = '\0';
  int opt_index = 0;
  static struct option long_opt[] = {
    { "help",   no_argument,       0, 'h' },
    { "port",   required_argument, 0, 'p' },
    // Sentinel
    { 0,        0,                 0,  0  }
  };
  // -------------------------------------------------------
  // args...
  // -------------------------------------------------------
  const char short_arg_list[] = "hp:";
  while(((unsigned char)opt != 255)) {
    opt = getopt_long_only(argc,
      argv,
      short_arg_list,
      long_opt,
      &opt_index);
    switch (opt) {
    case 'h': {
      print_usage(stdout, 0);
      break;
    }
    case 'p': {
      port = (uint16_t)(atoi(optarg));
      break;
    }
    case '?': {
      FATAL("unrecognized argument\n");
    }
    default: {
            break;
    }
    }
  }
  // -------------------------------------------------------
  // Create socket for incoming connections
  // -------------------------------------------------------
  int listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(listen_fd < 0) {
    FATAL("error socket() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  // -------------------------------------------------------
  // Set socket options
  // -Enable Socket reuse.
  // -------------------------------------------------------
#define _SET_SOCK_OPT(_fd, _level, _name, _val) do { \
  int _lval = _val; \
  if (setsockopt(_fd, _level, _name, &_lval, sizeof(_lval)) == -1) { \
    FATAL("setsockopt failed. Reason[%d]: %s\n", errno, strerror(errno)); \
  } } while(0)
  _SET_SOCK_OPT(listen_fd, SOL_SOCKET, SO_REUSEADDR, 1);
  _SET_SOCK_OPT(listen_fd, SOL_SOCKET, SO_REUSEPORT, 1);
  // -------------------------------------------------------
  // Construct local address structure
  // -------------------------------------------------------
  struct sockaddr_in sa = {
    .sin_family = AF_INET,
    .sin_port   = htons(port),
    .sin_addr   = {
      .s_addr = htonl(INADDR_ANY)
    }
  };
  // -------------------------------------------------------
  // Bind to the local address
  // -------------------------------------------------------
  if(bind(listen_fd, (struct sockaddr *)(&sa), sizeof(sa)) < 0) {
    FATAL("error bind() failed (port: %d) failed. Reason[%d]: %s\n",
      port, errno, strerror(errno));
  }
  // -------------------------------------------------------
  // Mark the socket so it will listen for
  // incoming connections
  // -------------------------------------------------------
  if(listen(listen_fd, _MAX_PENDING) < 0) {
    FATAL("error listen() failed (port: %d) failed. Reason[%d]: %s\n",
      port, errno, strerror(errno));
  }
  // -------------------------------------------------------
  // create epoll context
  // -------------------------------------------------------
  int epoll = epoll_create1(0);
  if (epoll < 0) {
    FATAL("error epoll_create1() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  // -------------------------------------------------------
  // setup listen_fd for epoll
  // -------------------------------------------------------
  struct epoll_event listen_ev;
  listen_ev.events = EPOLLIN;
  listen_ev.data.fd = listen_fd;
  if (epoll_ctl(epoll, EPOLL_CTL_ADD, listen_fd, &listen_ev)) {
    FATAL("error epoll_ctl() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  // -------------------------------------------------------
  // while events...
  // -------------------------------------------------------
  int nevents;
  struct epoll_event events[_NUM_EVENTS];
  while ((nevents = epoll_wait(epoll, events, _NUM_EVENTS, -1)) >= 0) {
    if (!nevents) {
      continue;
    }
    for (int n = 0; n < nevents; n++) {
      // ---------------------------------------------------
      // if event on server listen...
      // ---------------------------------------------------
      if (events[n].data.fd == listen_fd) {
        if (!_accept_conn(listen_fd, epoll)) {
          // error
        }
        continue;
      }
      // ---------------------------------------------------
      // client fd
      // ---------------------------------------------------
      conn_t* conn = (conn_t*)(events[n].data.ptr);
      int fd = conn->fd;
      //TRACE("fd: %d ptr: %p\n", fd, conn);
      // ---------------------------------------------------
      // read
      // ---------------------------------------------------
      char buf[1024];
      int nread = read(fd, buf, sizeof(buf));
      if (nread < 0) {
        ERROR("error read() failed. Reason[%d]: %s\n",
          errno, strerror(errno));
        _shutdown_conn(conn, epoll);
        continue;
      }
      else if (nread == 0) {
        //TRACE("closed[%d]\n", fd);
        _shutdown_conn(conn, epoll);
        continue;
      }
      //mem_display(buf, nread);
      // ---------------------------------------------------
      // parse
      // ---------------------------------------------------
      if(http_parser_execute(&(conn->hp), &(conn->hps), buf, nread) < nread) {
        ERROR("error http_parser_execute(). Reason: %s: %s\n",
          http_errno_name((enum http_errno)conn->hp.http_errno),
          http_errno_description((enum http_errno)conn->hp.http_errno));
        _shutdown_conn(conn, epoll);
      }
      //TRACE("path: %s\n", conn->path);
      //TRACE("ext:  %s\n", mime_type(conn->path, conn->path_len));
      // ---------------------------------------------------
      // open file
      // ---------------------------------------------------
      conn->file_fd = open(conn->path+1, O_RDONLY|O_NONBLOCK);
      if (conn->file_fd == -1) {
        ERROR("error open() failed. Reason[%d]: %s\n",
          errno, strerror(errno));
        _shutdown_conn(conn, epoll);
        continue;
      }
      struct stat file_stat;
      if(fstat(conn->file_fd, &file_stat) != 0) {
        ERROR("error fstat() failed. Reason[%d]: %s\n",
          errno, strerror(errno));
        _shutdown_conn(conn, epoll);
        continue;
      }
      conn->file_size = file_stat.st_size;
      // ---------------------------------------------------
      // send resp headers
      // ---------------------------------------------------
      conn->resp_len = snprintf(conn->resp,
        sizeof(conn->resp)-1,
        "HTTP/1.0 200 OK\r\n"
        "Server: %s\r\n"
        "Date: %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n"
        "Connection: close\r\n"
        "\r\n",
        "hignx/0.0.0",
        get_date_str(),
        mime_type(conn->path, conn->path_len),
        conn->file_size);
      ssize_t ss = send(conn->fd,
        conn->resp,
        conn->resp_len,
        MSG_NOSIGNAL);
      //TRACE("send(): %ld\n", ss);
      if (ss == -1) {
        ERROR("error send() failed. Reason[%d]: %s\n",
          errno, strerror(errno));
        _shutdown_conn(conn, epoll);
        continue;
      }
      // ---------------------------------------------------
      // sendfile
      // ---------------------------------------------------
      ssize_t sf = sendfile(conn->fd,
        conn->file_fd,
        &(conn->file_off),
        conn->file_size - conn->file_off);
      //TRACE("sendfile(): %ld\n", sf);
      if (sf == -1) {
        ERROR("error sendfile() failed. Reason[%d]: %s\n",
          errno, strerror(errno));
        _shutdown_conn(conn, epoll);
        continue;
      }
    }
  }
  // -------------------------------------------------------
  // cleanup...
  // -------------------------------------------------------
  shutdown(listen_fd, SHUT_RD);
  return 0;
}
