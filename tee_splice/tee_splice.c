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
//! \details main
//! \return  0 on success
//!          -1 on error
//! \param   argc/argv...
//! ----------------------------------------------------------------------------
int _handle_client(uint16_t a_port, int a_fd, int a_tee)
{
        int l_s;
        int l_fd = -1;
#define _BUF_SIZE (64*1024)
        // -------------------------------------------------
        // proxy socket
        // -------------------------------------------------
        errno = 0;
        l_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (l_fd < 0) {
                NDBG_ABORT("Error performing socket. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // connect to upstream
        // -------------------------------------------------
        struct sockaddr_storage l_sas;
        struct sockaddr_in* l_sa = (struct sockaddr_in*)(&l_sas);
        l_sa->sin_family = AF_INET;
        //l_sa->sin_addr.s_addr = inet_addr("127.0.0.1");
        //l_sa->sin_port = htons(a_port);
        l_sa->sin_addr.s_addr = inet_addr("74.6.143.26");
        l_sa->sin_port = htons(80);
        l_s = connect(l_fd, (struct sockaddr*)(&(l_sas)), sizeof(struct sockaddr_in));
        if (l_s < 0) {
                NDBG_ABORT("Error performing connect. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // use pipe
        // -------------------------------------------------
        int l_pfd[2];
        l_s = pipe(l_pfd);
        if (l_s < 0) {
                NDBG_ABORT("Error performing pipe. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // read client + write upstream
        // -------------------------------------------------
        while(1) {
                // -----------------------------------------
                // client->proxy
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(a_fd, NULL, l_pfd[1], NULL, _BUF_SIZE, SPLICE_F_MOVE);
                if (l_s < 0) {
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        break;
                }
                // -----------------------------------------
                // if tee...
                // -----------------------------------------
                if (a_tee) {
                        // ---------------------------------
                        // tee to output
                        // ---------------------------------
                        errno = 0;
                        l_s = tee(l_pfd[0], STDOUT_FILENO, _BUF_SIZE, 0);
                        if (l_s < 0) {
                                NDBG_ABORT("Error performing tee[%d]. Reason[%d]: %s\n",
                                           l_s, errno, strerror(errno));
                        }
                }
                // -----------------------------------------
                // proxy->server
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(l_pfd[0], NULL, l_fd, NULL, _BUF_SIZE, SPLICE_F_MOVE);
                if (l_s < 0) {
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        break;
                }
#if 1
                // -----------------------------------------
                // client->proxy
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(l_fd, NULL, l_pfd[1], NULL, _BUF_SIZE, SPLICE_F_MOVE);
                if (l_s < 0) {
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        break;
                }
                // -----------------------------------------
                // if tee...
                // -----------------------------------------
                if (a_tee) {
                        // ---------------------------------
                        // tee to output
                        // ---------------------------------
                        errno = 0;
                        l_s = tee(l_pfd[0], STDOUT_FILENO, _BUF_SIZE, 0);
                        if (l_s < 0) {
                                NDBG_ABORT("Error performing tee[%d]. Reason[%d]: %s\n",
                                           l_s, errno, strerror(errno));
                        }
                }
                // -----------------------------------------
                // proxy->server
                // splice
                // -----------------------------------------
                errno = 0;
                l_s = splice(l_pfd[0], NULL, a_fd, NULL, _BUF_SIZE, SPLICE_F_MOVE);
                if (l_s < 0) {
                        NDBG_ABORT("Error performing splice[%d]. Reason[%d]: %s\n",
                                   l_s, errno, strerror(errno));
                }
                else if (l_s == 0) {
                        break;
                }
#endif
        }
        close(l_fd);
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
        // -------------------------------------------------
        // accept loop
        // -------------------------------------------------
        struct sockaddr_in l_addr;
        socklen_t l_addr_len;
        while (1) {
                // ----------------------------------------
                // accept
                // ----------------------------------------
                errno = 0;
                l_s = accept(l_listen_fd, (struct sockaddr *) &l_addr, &l_addr_len);
                if (l_s < 0)
                {
                        NDBG_ABORT("Error performing accept. Reason[%d]: %s\n",
                                   errno, strerror(errno));
                }
                int l_fd = l_s;
                // ----------------------------------------
                // client loop
                // ----------------------------------------
                l_s = _handle_client(l_port, l_fd, l_tee);
                if (l_s != TS_STATUS_OK)
                {
                        // do nothing
                }
                // Try again...
                if (l_fd > 0 ) {close(l_fd); l_fd = -1;}
        }
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        if (l_listen_fd > 0) { close(l_listen_fd); l_listen_fd = -1; }
        return TS_STATUS_OK;
}
