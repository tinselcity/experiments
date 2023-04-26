//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <liburing.h>
#include "ndebug.h"
#include "ext/http-parser/http_parser.h"
#include "mime.h"
#include "date.h"
//! ----------------------------------------------------------------------------
//! constants
//! ----------------------------------------------------------------------------
#define _DEFAULT_PORT (12345)
#define _MAX_PENDING (1024)
#define _IO_URING_QUEUE_DEPTH (2048)
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
// ---------------------------------------------------------
// connection
// ---------------------------------------------------------
typedef struct {
  struct sockaddr_in sa;
  socklen_t sa_len;
  int fd;
  http_parser hp;
  http_parser_settings hps;
  char path[256];
  size_t path_len;
  char resp[1024];
  size_t resp_len;
  off_t resp_off;
  int file_fd;
  size_t file_size;
} hgx_conn_t;
// ---------------------------------------------------------
// hignx io uring io types
// ---------------------------------------------------------
typedef enum hgx_io_type {
  HGX_IO_TYPE_ACCEPT = 0,
  HGX_IO_TYPE_OPEN,
  HGX_IO_TYPE_STATX,
  HGX_IO_TYPE_READ,
  HGX_IO_TYPE_SEND,
  HGX_IO_TYPE_CLOSE
} hgx_io_type_t;
// ---------------------------------------------------------
// generic header for all types
// ---------------------------------------------------------
typedef struct hgx_io {
  hgx_io_type_t type;
  int fd;
} hgx_io_t;
// ---------------------------------------------------------
// accept
// ---------------------------------------------------------
typedef struct hgx_io_accept {
  hgx_io_t io;
  struct sockaddr_in sa;
  socklen_t sa_len;
} hgx_io_accept_t;
// ---------------------------------------------------------
// open
// ---------------------------------------------------------
typedef struct hgx_io_open {
  hgx_io_t io;
  hgx_conn_t* conn;
} hgx_io_open_t;
// ---------------------------------------------------------
// read
// ---------------------------------------------------------
typedef struct hgx_io_statx {
  hgx_io_t io;
  hgx_conn_t* conn;
  struct statx stat;
} hgx_io_statx_t;
// ---------------------------------------------------------
// read
// ---------------------------------------------------------
typedef struct hgx_io_read {
  hgx_io_t io;
  hgx_conn_t* conn;
  struct iovec iovec;
  char buf[1024];
} hgx_io_read_t;
// ---------------------------------------------------------
// read
// ---------------------------------------------------------
typedef struct hgx_io_send {
  hgx_io_t io;
  hgx_conn_t* conn;
} hgx_io_send_t;
// ---------------------------------------------------------
// close
// ---------------------------------------------------------
typedef struct hgx_io_close {
  hgx_io_t io;
  hgx_conn_t* conn;
} hgx_io_close_t;
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static hgx_io_accept_t* _new_io_accept(int fd) {
  hgx_io_accept_t* ptr = (hgx_io_accept_t*)calloc(sizeof(hgx_io_accept_t), 1);
  ptr->sa_len = sizeof(ptr->sa);
  ptr->io.type = HGX_IO_TYPE_ACCEPT;
  ptr->io.fd = fd;
  return ptr;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static hgx_io_open_t* _new_io_open(hgx_conn_t* conn) {
  hgx_io_open_t* ptr = (hgx_io_open_t*)calloc(sizeof(hgx_io_open_t), 1);
  ptr->io.type = HGX_IO_TYPE_OPEN;
  ptr->io.fd = conn->fd;
  ptr->conn = conn;
  return ptr;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static hgx_io_statx_t* _new_io_statx(hgx_conn_t* conn) {
  hgx_io_statx_t* ptr = (hgx_io_statx_t*)calloc(sizeof(hgx_io_statx_t), 1);
  ptr->io.type = HGX_IO_TYPE_STATX;
  ptr->io.fd = conn->fd;
  ptr->conn = conn;
  return ptr;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static hgx_io_read_t* _new_io_read(hgx_conn_t* conn) {
  hgx_io_read_t* ptr = (hgx_io_read_t*)calloc(sizeof(hgx_io_read_t), 1);
  ptr->io.type = HGX_IO_TYPE_READ;
  ptr->io.fd = conn->fd;
  ptr->iovec.iov_base = ptr->buf;
  ptr->iovec.iov_len = sizeof(ptr->buf);
  ptr->conn = conn;
  return ptr;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static hgx_io_send_t* _new_io_send(hgx_conn_t* conn) {
  hgx_io_send_t* ptr = (hgx_io_send_t*)calloc(sizeof(hgx_io_send_t), 1);
  ptr->io.type = HGX_IO_TYPE_SEND;
  ptr->io.fd = conn->fd;
  ptr->conn = conn;
  return ptr;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static hgx_io_close_t* _new_io_close(hgx_conn_t* conn) {
  hgx_io_close_t* ptr = (hgx_io_close_t*)calloc(sizeof(hgx_io_close_t), 1);
  ptr->io.type = HGX_IO_TYPE_CLOSE;
  ptr->io.fd = conn->fd;
  ptr->conn = conn;
  return ptr;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _hp_on_url(http_parser *parser, const char *at, size_t len)
{
  hgx_conn_t *conn = (hgx_conn_t*)(parser->data);
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
static void _reset_conn(hgx_conn_t* conn) {
  conn->fd = -1;
  conn->hps.on_url = _hp_on_url;
  http_parser_init(&(conn->hp), HTTP_REQUEST);
  conn->hp.data = conn;
  conn->resp_len = 0;
  conn->resp_off = 0;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static void _delete_conn(hgx_conn_t* conn) {
  free(conn);
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
  if(bind(listen_fd, (struct sockaddr*)(&sa), sizeof(sa)) < 0) {
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
  // create ring
  // -------------------------------------------------------
  struct io_uring ring;
  if (io_uring_queue_init(_IO_URING_QUEUE_DEPTH, &ring, 0) < 0) {
    FATAL("error io_uring_queue_init() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  // -------------------------------------------------------
  // prepare accept loop
  // -------------------------------------------------------
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    FATAL("error io_uring_get_sqe() failed.\n");
  }
  hgx_io_accept_t* io_accept = _new_io_accept(listen_fd);
  io_uring_prep_accept(sqe,
    io_accept->io.fd,
    (struct sockaddr*)&(io_accept->sa),
    &(io_accept->sa_len),
    0);
  io_uring_sqe_set_data(sqe, io_accept);

  if (io_uring_submit(&ring) < 0) {
    FATAL("error io_uring_submit() failed. Reason[%d]: %s\n",
      errno, strerror(errno));
  }
  // -------------------------------------------------------
  // completion queue event loop
  // -------------------------------------------------------
  struct io_uring_cqe* cqe;
  while (io_uring_wait_cqe(&ring, &cqe) >= 0) {
    if (!cqe->user_data) {
      ERROR("cqe->user_data null\n");
      continue;
    }
    hgx_io_t* io = (hgx_io_t*)(cqe->user_data);
    int res = cqe->res;
    // -----------------------------------------------------
    // if accept event
    // -----------------------------------------------------
    //TRACE("[TYPE: %d  RESULT: %d]\n", io->type, res);
    switch(io->type) {
    // -----------------------------------------------------
    // ACCEPT
    // -----------------------------------------------------
    case HGX_IO_TYPE_ACCEPT: {
      // ---------------------------------------------------
      // check for error
      // ---------------------------------------------------
      if (res < 0) {
        ERROR("error accept() failed. Reason[%d]: %s\n",
          -res, strerror(-res));
        goto resubmit_accept;
      }
      // ---------------------------------------------------
      // get rqst obj
      // ---------------------------------------------------
      //hgx_io_accept_t* rqst = (hgx_io_accept_t*)(cqe->user_data);
      //TRACE("connect(%d) from %s:%d\n",
      //  res,
      //  inet_ntoa(rqst->sa.sin_addr),
      //  ntohs(rqst->sa.sin_port));
      hgx_conn_t* conn = (hgx_conn_t*)calloc(sizeof(hgx_conn_t), 1);
      _reset_conn(conn);
      conn->fd = res;
      // ---------------------------------------------------
      // submit read request
      // ---------------------------------------------------
      hgx_io_read_t* io_read = _new_io_read(conn);
      sqe = io_uring_get_sqe(&ring);
      io_uring_prep_readv(sqe,
        io_read->io.fd,
        &(io_read->iovec),
        1,
        0);
      io_uring_sqe_set_data(sqe, io_read);
      io_uring_submit(&ring);
resubmit_accept:
      // ---------------------------------------------------
      // resubmit accept request
      // -reuse object
      // ---------------------------------------------------
      sqe = io_uring_get_sqe(&ring);
      io_uring_prep_accept(sqe,
        io_accept->io.fd,
        (struct sockaddr*)&(io_accept->sa),
        &(io_accept->sa_len),
        0);
      io_uring_sqe_set_data(sqe, io_accept);
      io_uring_submit(&ring);
      break;
    }
    // -----------------------------------------------------
    // OPEN
    // -----------------------------------------------------
    case HGX_IO_TYPE_STATX: {
      // ---------------------------------------------------
      // check for error
      // ---------------------------------------------------
      //TRACE("statx(%d)\n", res);
      hgx_io_statx_t* rqst = (hgx_io_statx_t*)(cqe->user_data);
      if (res < 0) {
        ERROR("error statx() failed. Reason[%d]: %s\n",
          -res, strerror(-res));
        free(rqst);
        // TODO _shutdown_conn routine...(submit close)
        break;
      }
      hgx_conn_t* conn = rqst->conn;
      conn->file_size = rqst->stat.stx_size;
      //TRACE("conn->file_size: %lu\n", conn->file_size);
      free(rqst);
      // ---------------------------------------------------
      // submit open request
      // ---------------------------------------------------
    	struct open_how how;
      memset(&how, 0, sizeof(how));
      how.flags = O_RDONLY;
      hgx_io_open_t* io_open = _new_io_open(conn);
      sqe = io_uring_get_sqe(&ring);
      io_uring_prep_openat2(sqe,
        AT_FDCWD,
        conn->path+1,
        &how);
      io_uring_sqe_set_data(sqe, io_open);
      io_uring_submit(&ring);
      break;
    }
    // -----------------------------------------------------
    // OPEN
    // -----------------------------------------------------
    case HGX_IO_TYPE_OPEN: {
      // ---------------------------------------------------
      // check for error
      // ---------------------------------------------------
      //TRACE("open(%d)\n", res);
      hgx_io_open_t* rqst = (hgx_io_open_t*)(cqe->user_data);
      //TRACE("rqst: %p\n", rqst);
      if (res < 0) {
        ERROR("error accept() failed. Reason[%d]: %s\n",
          -res, strerror(-res));
        free(rqst);
        // TODO _shutdown_conn routine...(submit close)
        break;
      }
      hgx_conn_t* conn = rqst->conn;
      conn->file_fd = res;
      free(rqst);
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
      hgx_io_send_t* io_send = _new_io_send(conn);
      sqe = io_uring_get_sqe(&ring);
      io_uring_prep_send(sqe,
        io_send->io.fd,
        conn->resp,
        conn->resp_len,
        0);
      // link send with subsequent splice submissions
      sqe->flags |= IOSQE_IO_LINK;
      io_uring_sqe_set_data(sqe, io_send);
      io_uring_submit(&ring);
      // ---------------------------------------------------
      // submit splice
      // ---------------------------------------------------
      // TODO
      // - pipe2 (can't use io_uring)
      // - splice
#if 0
      // splice file data to pipe
      io_uring_push_splice((struct io_uring *)conn->svr->ring,
        res.file_fd,
        0,
        conn->svr->pipefds[1],
        -1,
        res.file_sz,
        (void *)conn,
        IOSQE_IO_LINK);

      // splice pipe to client conn
      io_uring_push_splice((struct io_uring *)conn->svr->ring,
        conn->svr->pipefds[0],
        -1,
        conn->fd,
        -1,
        res.file_sz,
        (void *)conn,
        0);
#endif
      break;
    }
    // -----------------------------------------------------
    // READ
    // -----------------------------------------------------
    case HGX_IO_TYPE_READ: {
      hgx_io_read_t* rqst = (hgx_io_read_t*)(cqe->user_data);
      hgx_conn_t* conn = rqst->conn;
      //TRACE("read(%d) from fd: %d\n", res, rqst->io.fd);
      // ---------------------------------------------------
      // check for error
      // ---------------------------------------------------
      if (res < 0) {
        free(rqst);
        hgx_io_close_t* io_close = _new_io_close(conn);
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_close(sqe, io_close->io.fd);
        io_uring_sqe_set_data(sqe, io_close);
        io_uring_submit(&ring);
        break;
      }
      // ---------------------------------------------------
      // check for close
      // ---------------------------------------------------
      else if (res == 0) {
        free(rqst);
        hgx_io_close_t* io_close = _new_io_close(conn);
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_close(sqe, io_close->io.fd);
        io_uring_sqe_set_data(sqe, io_close);
        io_uring_submit(&ring);
        break;
      }
      //mem_display(rqst->buf, (size_t)res);
      // ---------------------------------------------------
      // parse
      // ---------------------------------------------------
      size_t nread = (size_t)res;
      if(http_parser_execute(&(conn->hp), &(conn->hps), rqst->buf, nread) < nread) {
        ERROR("error http_parser_execute(). Reason: %s: %s\n",
          http_errno_name((enum http_errno)conn->hp.http_errno),
          http_errno_description((enum http_errno)conn->hp.http_errno));
        hgx_io_close_t* io_close = _new_io_close(conn);
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_close(sqe, io_close->io.fd);
        io_uring_sqe_set_data(sqe, io_close);
        io_uring_submit(&ring);
        break;
      }
      //TRACE("path: %s\n", conn->path);
      //TRACE("ext:  %s\n", mime_type(conn->path, conn->path_len));
      // ---------------------------------------------------
      // submit stat rqst to get file size
      // ---------------------------------------------------
      hgx_io_statx_t* io_statx = _new_io_statx(rqst->conn);
      sqe = io_uring_get_sqe(&ring);
      io_uring_prep_statx(sqe,
        AT_FDCWD,
        conn->path+1,
        0,
        STATX_SIZE,
        &(io_statx->stat));
      io_uring_sqe_set_data(sqe, io_statx);
      io_uring_submit(&ring);
      // ---------------------------------------------------
      // make new read
      // reuse last read request
      // ---------------------------------------------------
      sqe = io_uring_get_sqe(&ring);
      io_uring_prep_readv(sqe,
        rqst->io.fd,
        &(rqst->iovec),
        1,
        0);
      io_uring_sqe_set_data(sqe, rqst);
      io_uring_submit(&ring);
      break;
    }
    // -----------------------------------------------------
    // WRITE
    // -----------------------------------------------------
    case HGX_IO_TYPE_SEND: {
      // ---------------------------------------------------
      // check for error
      // ---------------------------------------------------
      if (res < 0) {
        // TODO free send request???
#if 0
        hgx_io_close_t* io_close = _new_io_close(fd);
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_close(sqe, io_close->io.fd);
        io_uring_sqe_set_data(sqe, io_close);
        io_uring_submit(&ring);
#endif
        break;
      }
      // TODO free send request???
      break;
    }
    // -----------------------------------------------------
    // CLOSE
    // -----------------------------------------------------
    case HGX_IO_TYPE_CLOSE: {
      hgx_io_close_t* rqst = (hgx_io_close_t*)(cqe->user_data);
      _delete_conn(rqst->conn);
      free(rqst);
      break;
    }
    // -----------------------------------------------------
    // default
    // -----------------------------------------------------
    default: {
      break;
    }
    }
    // mark entry as seen -return for reuse in ring buffer
    io_uring_cqe_seen(&ring, cqe);
  }
  FATAL("error io_uring_wait_cqe() failed. Reason[%d]: %s\n",
    errno, strerror(errno));
  free(io_accept);
}
