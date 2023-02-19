//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
//! ----------------------------------------------------------------------------
//! STATUS
//! ----------------------------------------------------------------------------
#ifndef TS_STATUS_OK
#define TS_STATUS_OK 0
#endif
#ifndef TS_STATUS_ERROR
#define TS_STATUS_ERROR -1
#endif
#ifndef TS_STATUS_DONE
#define TS_STATUS_DONE -2
#endif
#define _BUF_SIZE (64*1024)
//! ----------------------------------------------------------------------------
//! ANSI Color Code Strings
//! ----------------------------------------------------------------------------
#define ANSI_COLOR_OFF          "\033[0m"
#define ANSI_COLOR_FG_BLACK     "\033[01;30m"
#define ANSI_COLOR_FG_RED       "\033[01;31m"
#define ANSI_COLOR_FG_GREEN     "\033[01;32m"
#define ANSI_COLOR_FG_YELLOW    "\033[01;33m"
#define ANSI_COLOR_FG_BLUE      "\033[01;34m"
#define ANSI_COLOR_FG_MAGENTA   "\033[01;35m"
#define ANSI_COLOR_FG_CYAN      "\033[01;36m"
#define ANSI_COLOR_FG_WHITE     "\033[01;37m"
#define ANSI_COLOR_FG_DEFAULT   "\033[01;39m"
#define ANSI_COLOR_BG_BLACK     "\033[01;40m"
#define ANSI_COLOR_BG_RED       "\033[01;41m"
#define ANSI_COLOR_BG_GREEN     "\033[01;42m"
#define ANSI_COLOR_BG_YELLOW    "\033[01;43m"
#define ANSI_COLOR_BG_BLUE      "\033[01;44m"
#define ANSI_COLOR_BG_MAGENTA   "\033[01;45m"
#define ANSI_COLOR_BG_CYAN      "\033[01;46m"
#define ANSI_COLOR_BG_WHITE     "\033[01;47m"
#define ANSI_COLOR_BG_DEFAULT   "\033[01;49m"
//! ----------------------------------------------------------------------------
//! debug macros
//! ----------------------------------------------------------------------------
#ifndef NDBG_OUTPUT
#define NDBG_OUTPUT(...) do { \
                fprintf(stdout, __VA_ARGS__); \
                fflush(stdout); \
        } while(0)
#endif
#ifndef NDBG_PRINT
#define NDBG_PRINT(...) do { \
                fprintf(stdout, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
                fprintf(stdout, __VA_ARGS__);               \
                fflush(stdout); \
        } while(0)
#endif
#ifndef NDBG_ERROR
#define NDBG_ERROR(...) do { \
                fprintf(stderr, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
                fprintf(stderr, __VA_ARGS__);               \
                fflush(stderr); \
        } while(0)
#endif
#ifndef NDBG_ABORT
#define NDBG_ABORT(...) do { \
                fprintf(stderr, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
                fprintf(stderr, __VA_ARGS__);               \
                fflush(stderr); \
                return TS_STATUS_ERROR; \
        } while(0)
#endif
#ifndef NDBG_HEXDUMP
#define NDBG_HEXDUMP(buffer, len) do { \
                _mem_display((const uint8_t*)buffer, len); \
                fflush(stdout); \
        } while(0)
#endif
#ifndef UNUSED
#define UNUSED(x) ( (void)(x) )
#endif
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void _mem_display(const uint8_t* a_buf, size_t a_len)
{
#define _LINE_SIZE 256
        char l_line[_LINE_SIZE+1] = "";
        uint32_t l_num_bytes = 0;
        char l_b_display[8] = "";
        char l_ascii[17] = "";
        while (l_num_bytes < a_len) {
                uint32_t l_col = 0;
                snprintf(l_line,
                         _LINE_SIZE,
                         "%s0x%08x %s",
                         ANSI_COLOR_FG_BLUE, l_num_bytes, ANSI_COLOR_OFF);
                strncat(l_line, " ", _LINE_SIZE);
                strncat(l_line, ANSI_COLOR_FG_GREEN, _LINE_SIZE);
                while ((l_col < 16) &&
                       (l_num_bytes < a_len)) {
                        snprintf(l_b_display,
                                 8,
                                 "%02x",
                                 (unsigned char) a_buf[l_num_bytes]);
                        strncat(l_line, l_b_display, _LINE_SIZE);
                        if (isprint(a_buf[l_num_bytes])) {
                                l_ascii[l_col] = a_buf[l_num_bytes];
                        }
                        else {
                                l_ascii[l_col] = '.';
                        }
                        l_col++;
                        l_num_bytes++;
                        if (!(l_col % 4)) {
                                strncat(l_line, " ", _LINE_SIZE);
                        }
                }
                if ((l_col < 16) &&
                    (l_num_bytes >= a_len)) {
                        while (l_col < 16) {
                                strncat(l_line, "..", _LINE_SIZE);
                                l_ascii[l_col] = '.';
                                l_col++;
                                if (!(l_col % 4)) {
                                        strncat(l_line, " ", _LINE_SIZE);
                                }
                        }
                }
                l_ascii[l_col] = '\0';
                strncat(l_line, ANSI_COLOR_OFF, _LINE_SIZE);
                strncat(l_line, " ", _LINE_SIZE);
                strncat(l_line, l_ascii, _LINE_SIZE);
                NDBG_OUTPUT("%s\n", l_line);
        }
}
//! ----------------------------------------------------------------------------
//! client
//! ----------------------------------------------------------------------------
typedef struct {
        int m_client_fd;
        int m_proxy_fd;
        int m_pipe_fd[2];
} client_t;
//! ----------------------------------------------------------------------------
//! \details TODO
//! \return  0 on success
//!          -1 on error
//! \param   TODO
//! ----------------------------------------------------------------------------
int _setup_client(client_t* a_client, int a_client_fd)
{
        int l_s;
        int l_flags;
        a_client->m_client_fd = a_client_fd;
        // -------------------------------------------------
        // proxy socket
        // -------------------------------------------------
        errno = 0;
        a_client->m_proxy_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (a_client->m_proxy_fd < 0) {
                NDBG_ABORT("Error performing socket. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // use pipe
        // -------------------------------------------------
        errno = 0;
        l_s = pipe(a_client->m_pipe_fd);
        if (l_s < 0) {
                NDBG_ABORT("Error performing pipe. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // SET_NONBLOCK
        // -------------------------------------------------
#define _SET_NONBLOCK(_fd) do { \
                errno = 0; \
                l_flags = fcntl(_fd, F_GETFL, 0); \
                if (l_flags == -1) { \
                        NDBG_ABORT("Error performing fcntl() Reason[%d]: %s\n", errno, strerror(errno)); \
                } \
                errno = 0; \
                if (fcntl(_fd, F_SETFL, l_flags | O_NONBLOCK) < 0) { \
                        NDBG_ABORT("Error performing fcntl() Reason[%d]: %s\n", errno, strerror(errno)); \
                } \
} while(0)
        _SET_NONBLOCK(a_client->m_client_fd);
        _SET_NONBLOCK(a_client->m_proxy_fd);
        _SET_NONBLOCK(a_client->m_pipe_fd[0]);
        _SET_NONBLOCK(a_client->m_pipe_fd[1]);
        // -------------------------------------------------
        // setup address
        // -------------------------------------------------
        struct sockaddr_storage l_sas;
        struct sockaddr_in* l_sa = (struct sockaddr_in*)(&l_sas);
        l_sa->sin_family = AF_INET;
        //l_sa->sin_addr.s_addr = inet_addr("127.0.0.1");
        //l_sa->sin_port = htons(a_port);
        l_sa->sin_addr.s_addr = inet_addr("74.6.143.26");
        l_sa->sin_port = htons(80);
        // -------------------------------------------------
        // connect
        // -------------------------------------------------
        while (1) {
                errno = 0;
                l_s = connect(a_client->m_proxy_fd, (struct sockaddr*)(&(l_sas)), sizeof(struct sockaddr_in));
                if (l_s < 0) {
                        if ((errno == EINPROGRESS) ||
                            (errno == EALREADY) ||
                            (errno == EAGAIN)) {
                                usleep(1000);
                                continue;
                        }
                        NDBG_ABORT("Error performing connect. Reason[%d]: %s\n",
                                   errno, strerror(errno));
                }
                break;
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int _client_teardown(client_t* a_client, fd_set* a_rd_fdset, fd_set* a_wr_fdset)
{
        if (a_client->m_client_fd >= 0) {
                FD_CLR(a_client->m_client_fd, a_rd_fdset);
                FD_CLR(a_client->m_client_fd, a_wr_fdset);
                close(a_client->m_client_fd);
                a_client->m_client_fd = -1;
        }
        if (a_client->m_proxy_fd >= 0) {
                FD_CLR(a_client->m_proxy_fd, a_rd_fdset);
                FD_CLR(a_client->m_proxy_fd, a_wr_fdset);
                close(a_client->m_proxy_fd);
                a_client->m_proxy_fd = -1;
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int _client_readable(client_t* a_client, int a_tee)
{
        int l_s;
        // -------------------------------------------------
        // read until EAGAIN or ERROR
        // -------------------------------------------------
        while(1) {
                // -----------------------------------------
                // client->proxy
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_client->m_client_fd,
                             NULL, a_client->m_pipe_fd[1],
                             NULL, _BUF_SIZE,
                             SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
                if (l_s < 0) {
                        if (errno == EAGAIN) {
                                break;
                        }
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        return TS_STATUS_DONE;
                }
                // -----------------------------------------
                // if tee...
                // -----------------------------------------
                if (a_tee) {
                        // ---------------------------------
                        // tee to output
                        // ---------------------------------
                        errno = 0;
                        l_s = tee(a_client->m_pipe_fd[0],
                                  STDOUT_FILENO,
                                  _BUF_SIZE,
                                  SPLICE_F_NONBLOCK);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        break;
                                }
                                NDBG_ABORT("Error performing tee[%d]. Reason[%d]: %s\n",
                                           l_s, errno, strerror(errno));
                        }
                }
#if 1
                // -----------------------------------------
                // proxy->server
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_client->m_pipe_fd[0],
                             NULL,
                             a_client->m_proxy_fd,
                             NULL,
                             _BUF_SIZE,
                             SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
                if (l_s < 0) {
                        if (errno == EAGAIN) {
                                break;
                        }
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        return TS_STATUS_DONE;
                }
#endif
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int _client_writeable(client_t* a_client, int a_tee)
{
#if 0
        int l_s;
        // -------------------------------------------------
        // read upstream / write client
        // -------------------------------------------------
        while(1) {
                // -----------------------------------------
                // proxy->server
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_client->m_pipe_fd[0],
                             NULL,
                             a_client->m_client_fd,
                             NULL,
                             _BUF_SIZE,
                             SPLICE_F_MOVE);
                if (l_s < 0) {
                        if (errno == EAGAIN) {
                                break;
                        }
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        return TS_STATUS_DONE;
                }
        }
#endif
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int _proxy_readable(client_t* a_client, int a_tee)
{
        int l_s;
        // -------------------------------------------------
        // read until EAGAIN or ERROR
        // -------------------------------------------------
        while(1) {
                // -----------------------------------------
                // client->proxy
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_client->m_proxy_fd,
                             NULL,
                             a_client->m_pipe_fd[1],
                             NULL,
                             _BUF_SIZE,
                             SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
                if (l_s < 0) {
                        if (errno == EAGAIN) {
                                break;
                        }
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        return TS_STATUS_DONE;
                }
                // -----------------------------------------
                // if tee...
                // -----------------------------------------
                if (a_tee) {
                        // ---------------------------------
                        // tee to output
                        // ---------------------------------
                        errno = 0;
                        l_s = tee(a_client->m_pipe_fd[0],
                                  STDOUT_FILENO,
                                  _BUF_SIZE,
                                  SPLICE_F_NONBLOCK);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        break;
                                }
                                NDBG_ABORT("Error performing tee[%d]. Reason[%d]: %s\n",
                                           l_s, errno, strerror(errno));
                        }
                }
#if 1
                // -----------------------------------------
                // proxy->server
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_client->m_pipe_fd[0],
                             NULL,
                             a_client->m_client_fd,
                             NULL,
                             _BUF_SIZE,
                             SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
                if (l_s < 0) {
                        if (errno == EAGAIN) {
                                break;
                        }
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        return TS_STATUS_DONE;
                }
#endif
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int _proxy_writeable(client_t* a_client, int a_tee)
{
#if 0
        int l_s;
        // -------------------------------------------------
        // read upstream / write client
        // -------------------------------------------------
        while(1) {
                // -----------------------------------------
                // proxy->server
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_client->m_pipe_fd[0],
                             NULL,
                             a_client->m_proxy_fd,
                             NULL,
                             _BUF_SIZE,
                             SPLICE_F_MOVE);
                if (l_s < 0) {
                        if (errno == EAGAIN) {
                                break;
                        }
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        return TS_STATUS_DONE;
                }
        }
#endif
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: Print the command line help.
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void print_usage(FILE* a_stream, int a_exit_code)
{
        fprintf(a_stream, "Usage: tee_splice [options]\n");
        fprintf(a_stream, "Options:\n");
        fprintf(a_stream, "  -h, --help    display this help and exit.\n");
        fprintf(a_stream, "  -l, --listen  port to listen on\n");
        fprintf(a_stream, "  -p, --proxy   port to proxy to\n");
        fprintf(a_stream, "  -t, --tee     tee to stdout\n");
        exit(a_exit_code);
}
//! ----------------------------------------------------------------------------
//! \details main
//! \return  0 on success
//!          -1 on error
//! \param   argc/argv...
//! ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
        uint16_t l_listen = 0;
        uint16_t l_port = 0;
        int l_tee = 0;
        // -------------------------------------------------
        // arg defs
        // -------------------------------------------------
        char l_opt = '\0';
        int l_opt_index = 0;
        static struct option l_long_opt[] = {
                { "help",   no_argument,       0, 'h' },
                { "listen", required_argument, 0, 'l' },
                { "port",   required_argument, 0, 'p' },
                { "tee",    no_argument,       0, 't' },
               // Sentinel
                { 0,        0,                 0,  0  }
        };
        // -------------------------------------------------
        // args...
        // -------------------------------------------------
        const char l_short_arg_list[] = "hl:p:t";
        while(((unsigned char)l_opt != 255)) {
                l_opt = getopt_long_only(argc,
                                         argv,
                                         l_short_arg_list,
                                         l_long_opt,
                                         &l_opt_index);
                switch (l_opt) {
                case 'h': {
                        print_usage(stdout, 0);
                        break;
                }
                case 'l': {
                        l_listen = (uint16_t)(atoi(optarg));
                        break;
                }
                case 'p': {
                        l_port = (uint16_t)(atoi(optarg));
                        break;
                }
                case 't': {
                        l_tee = 1;
                        break;
                }
                case '?': {
                        NDBG_ABORT("unrecognized argument.  Exiting.\n");
                }
                default: {
                        break;
                }
                }
        }
        // -------------------------------------------------
        // check args
        // -------------------------------------------------
        if (!l_listen ||
            !l_port) {
                NDBG_ABORT("listen port and proxy port must be specified.\n");
        }
        // -------------------------------------------------
        // if tee ensure stdout is pipe
        // -------------------------------------------------
        int l_s;
        if (l_tee) {
                struct stat l_stat;
                errno = 0;
                l_s = fstat(STDOUT_FILENO, &l_stat);
                if (l_s < 0) {
                        NDBG_ABORT("error performing fstat.  Reason: %s\n", strerror(errno));
                }
                if (!S_ISFIFO(l_stat.st_mode)) {
                        NDBG_ABORT("error stdout must be a pipe\n");
                }
        }
        // -------------------------------------------------
        // socket
        // -------------------------------------------------
        int l_listen_fd = -1;
        errno = 0;
        l_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (l_listen_fd < 0) {
                NDBG_ABORT("Error performing socket. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // reuse addr
        // -------------------------------------------------
        int l_val = 1;
        errno = 0;
        l_s = setsockopt(l_listen_fd, SOL_SOCKET, SO_REUSEADDR, &l_val, sizeof(l_val));
        if (l_s < 0) {
                NDBG_ABORT("Error setsockopt() Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
        // -------------------------------------------------
        // reuse port
        // -------------------------------------------------
        errno = 0;
        l_s = setsockopt(l_listen_fd, SOL_SOCKET, SO_REUSEPORT, &l_val, sizeof(l_val));
        if (l_s < 0) {
                NDBG_ABORT("Error setsockopt() Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
        // -------------------------------------------------
        // no delay
        // -------------------------------------------------
        errno = 0;
        l_s = setsockopt(l_listen_fd, SOL_SOCKET, TCP_NODELAY, &l_val, sizeof(l_val));
        if (l_s < 0) {
                NDBG_ABORT("Error setsockopt() Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
        // -------------------------------------------------
        // set zero copy
        // requires setcap ?
        // -------------------------------------------------
        errno = 0;
        l_s = setsockopt(l_listen_fd, SOL_SOCKET, SO_ZEROCOPY, &l_val, sizeof(l_val));
        if (l_s < 0) {
                NDBG_ABORT("Error setsockopt() Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
        // -------------------------------------------------
        // bind listen on localhost <listen-port>
        // -------------------------------------------------
        struct sockaddr_in l_sa;
        l_sa.sin_family = AF_INET;
        l_sa.sin_addr.s_addr = inet_addr("127.0.0.1");
        l_sa.sin_port = htons(l_listen);
        errno = 0;
        l_s = bind(l_listen_fd, (struct sockaddr *)&l_sa, sizeof((l_sa)));
        if(l_s < 0) {
                NDBG_ABORT("Error bind() failed (port: %d). Reason[%d]: %s\n",
                           l_listen, errno, strerror(errno));
        }
        // -------------------------------------------------
        // listen max 64 backlog
        // -------------------------------------------------
        l_s = listen(l_listen_fd, 64);
        if (l_s < 0) {
                NDBG_ABORT("Error performing listen. Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
        // -------------------------------------------------
        // read / write set
        // -------------------------------------------------
        fd_set l_rd_fdset;
        fd_set l_wr_fdset;
        FD_ZERO(&l_rd_fdset);
        FD_ZERO(&l_wr_fdset);
        FD_SET(l_listen_fd, &l_rd_fdset);
        // -------------------------------------------------
        // client
        // -------------------------------------------------
        client_t l_client;
        l_client.m_proxy_fd = -1;
        l_client.m_client_fd = -1;
        l_client.m_pipe_fd[0] = -1;
        l_client.m_pipe_fd[1] = -1;
        // -------------------------------------------------
        // accept loop
        // -------------------------------------------------
        struct sockaddr_in l_addr;
        socklen_t l_addr_len;
        int l_max_fd = l_listen_fd;
        while (1) {
                fd_set l_tmp_rd_fdset = l_rd_fdset;
                fd_set l_tmp_wr_fdset = l_wr_fdset;
                // ----------------------------------------
                // select on fd
                // ----------------------------------------
                errno = 0;
                l_s = select(l_max_fd+1,
                             &l_tmp_rd_fdset,
                             &l_tmp_wr_fdset,
                             NULL,
                             NULL);
                if (l_s < 0) {
                        NDBG_ABORT("Error performing select[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                // ----------------------------------------
                // if is set
                // ----------------------------------------
                if(FD_ISSET(l_listen_fd, &l_tmp_rd_fdset)) {
                        // ---------------------------------
                        // accept
                        // ---------------------------------
                        errno = 0;
                        l_s = accept4(l_listen_fd,
                                      (struct sockaddr *)&l_addr,
                                      &l_addr_len,
                                      SOCK_NONBLOCK|SOCK_CLOEXEC);
                        if (l_s < 0) {
                                NDBG_ABORT("Error performing accept. Reason[%d]: %s\n",
                                           errno, strerror(errno));
                        }
                        // ---------------------------------
                        // Set the fd to no-delay mode
                        // ---------------------------------
                        int l_client_fd = l_s;
                        // ---------------------------------
                        // create client
                        // ---------------------------------
                        l_s = _setup_client(&l_client, l_client_fd);
                        if (l_s != TS_STATUS_OK) {
                                l_s = _client_teardown(&l_client, &l_rd_fdset, &l_wr_fdset);
                                UNUSED(l_s);
                                l_max_fd = l_listen_fd + 1;
                                continue;
                        }
                        // ---------------------------------
                        // set max fd
                        // ---------------------------------
                        if (l_client.m_client_fd > l_max_fd) {
                                l_max_fd = l_client.m_client_fd;
                        }
                        if (l_client.m_proxy_fd > l_max_fd) {
                                l_max_fd = l_client.m_proxy_fd;
                        }
                        // ---------------------------------
                        // set flags
                        // ---------------------------------
                        FD_SET(l_client.m_client_fd, &l_rd_fdset);
                        FD_SET(l_client.m_client_fd, &l_wr_fdset);
                        FD_SET(l_client.m_proxy_fd, &l_rd_fdset);
                        FD_SET(l_client.m_proxy_fd, &l_wr_fdset);
                }
#define _TEARDOWN_CHECK(_s) do { \
                if (_s != TS_STATUS_OK) { \
                        int _l_s = _client_teardown(&l_client, &l_rd_fdset, &l_wr_fdset); \
                        UNUSED(_l_s); \
                        l_max_fd = l_listen_fd + 1; \
                        continue; \
                } } while(0)
                // ----------------------------------------
                // client readable
                // ----------------------------------------
                if(FD_ISSET(l_client.m_client_fd, &l_tmp_rd_fdset)) {
                        l_s = _client_readable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                        l_s = _proxy_writeable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                }
                // ----------------------------------------
                // client writeable
                // ----------------------------------------
                if(FD_ISSET(l_client.m_client_fd, &l_tmp_wr_fdset)) {
                        l_s = _client_writeable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                        l_s = _proxy_readable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                }
                // ----------------------------------------
                // proxy readable
                // ----------------------------------------
                if(FD_ISSET(l_client.m_proxy_fd, &l_tmp_rd_fdset)) {
                        l_s = _proxy_readable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                        l_s = _client_writeable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                }
                // ----------------------------------------
                // proxy writeable
                // ----------------------------------------
                if(FD_ISSET(l_client.m_proxy_fd, &l_tmp_wr_fdset)) {
                        l_s = _proxy_writeable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                        l_s = _client_readable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                }
        }
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        if (l_listen_fd > 0) { close(l_listen_fd); l_listen_fd = -1; }
        return TS_STATUS_OK;
}
