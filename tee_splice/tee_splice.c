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
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#define _USE_NCURSES 1
#ifdef _USE_NCURSES
#include <ncurses.h>
#endif
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
#define NDBG_HEXDUMP(_buf, _len, _off) do { \
                _mem_display((const uint8_t*)_buf, _len, _off); \
                fflush(stdout); \
        } while(0)
#endif
#ifndef UNUSED
#define UNUSED(x) ( (void)(x) )
#endif
//! ----------------------------------------------------------------------------
//! socks state
//! ----------------------------------------------------------------------------
typedef enum {
        SOCKS_STATE_NONE = 0,
        SOCKS_STATE_CLIENT_CONNECT,
        SOCKS_STATE_UPSTREAM_CONNECT,
        SOCKS_STATE_CONNECTED,
} socks_state_t;
//! ----------------------------------------------------------------------------
//! client
//! ----------------------------------------------------------------------------
typedef struct {
        int m_client_fd;
        int m_proxy_fd;
        int m_pipe_fd[2];
        int m_pipe_rd_fd[2];
        socks_state_t m_socks_state;
        struct sockaddr_storage m_proxy_sas;
        uint8_t m_client_connect_buf[18];
        size_t m_client_connect_buf_len;
} client_t;
//! ----------------------------------------------------------------------------
//! globals
//! ----------------------------------------------------------------------------
#ifdef _USE_NCURSES
int g_curses = 0;
WINDOW *g_win_rqst = NULL;
WINDOW *g_win_resp = NULL;
int g_term_ws_row = 0;
int g_term_ws_col = 0;
int g_term_ws_changed = 0;
#endif
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
#ifdef _USE_NCURSES
void _handle_winch(int a_sig)
{
        struct winsize l_ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &l_ws);
        g_term_ws_row = l_ws.ws_row;
        g_term_ws_col = l_ws.ws_col;
        g_term_ws_changed = 1;
}
#endif
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void _mem_display(const uint8_t* a_buf, size_t a_len, size_t a_off)
{
#define _LINE_SIZE 256
        uint32_t l_line_num = a_off/16;
        char l_line[_LINE_SIZE+1] = "";
        uint32_t l_off = a_off;
        uint32_t l_num_bytes = 0;
        char l_b_display[8] = "";
        char l_ascii[17] = "";
        while (l_num_bytes < a_len) {
                uint32_t l_col = 0;
                snprintf(l_line,
                         _LINE_SIZE,
                         "%s0x%08x %s",
                         ANSI_COLOR_FG_BLUE, l_off, ANSI_COLOR_OFF);
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
                        ++l_col;
                        ++l_num_bytes;
                        ++l_off;
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
                ++l_line_num;
        }
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
#ifdef _USE_NCURSES
void _clear_display_win(WINDOW* a_win)
{
        // -------------------------------------------------
        // clear
        // -------------------------------------------------
        if (g_curses &&
            a_win) {
                werase(a_win);
                box(a_win, 0, 0);
                if (a_win == g_win_rqst) {
                        wattron(a_win, COLOR_PAIR(5));
                        box(a_win, 0, 0);
                        wattron(a_win, WA_BOLD);
                        mvwprintw(a_win, 0, 1, "Request");
                        wattroff(a_win, WA_BOLD);
                }
                else {
                        wattron(a_win, COLOR_PAIR(4));
                        box(a_win, 0, 0);
                        wattron(a_win, WA_BOLD);
                        mvwprintw(a_win, 0, 1, "Response");
                        wattroff(a_win, WA_BOLD);
                }
        }
}
#endif
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
#ifdef _USE_NCURSES
void _mem_display_win(const uint8_t* a_buf, size_t a_len, size_t a_off, WINDOW* a_win)
{
#define _LINE_SIZE 256
        uint32_t l_line_num = a_off/16;
        char l_line[_LINE_SIZE+1] = "";
        uint32_t l_off = a_off;
        uint32_t l_num_bytes = 0;
        char l_b_display[8] = "";
        char l_ascii[17] = "";
        while (l_num_bytes < a_len) {
                uint32_t l_col = 0;
                // -----------------------------------------
                // display address
                // -----------------------------------------
                char l_addr[24];
                snprintf(l_addr,
                         24,
                         "0x%08x ",
                         l_off);
                strncat(l_addr, " ", 23);
                if (g_curses &&
                    a_win) {
                        if ((l_line_num+2) < g_term_ws_row) {
                                wattron(a_win, WA_BOLD);
                                wattron(a_win, COLOR_PAIR(1));
                                mvwprintw(a_win, l_line_num+1, 1, "%s", l_addr);
                                wattroff(a_win, WA_BOLD);
                        }
                }
                else {
                        NDBG_OUTPUT("%s", l_addr);
                }
                // -----------------------------------------
                // display hex
                // -----------------------------------------
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
                        ++l_col;
                        ++l_num_bytes;
                        ++l_off;
                        if (!(l_col % 4)) {
                                strncat(l_line, " ", _LINE_SIZE);
                        }
                }
                // -----------------------------------------
                // display ascii
                // -----------------------------------------
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
                if (g_curses &&
                    a_win) {
                        if ((l_line_num+2) < g_term_ws_row) {
                                wattron(a_win, WA_BOLD);
                                wattron(a_win, COLOR_PAIR(2));
                                mvwprintw(a_win, l_line_num+1, 1+strlen(l_addr), "%s", l_line);
                                wattroff(a_win, WA_BOLD);
                                wattron(a_win, COLOR_PAIR(3));
                                mvwprintw(a_win, l_line_num+1, 1+strlen(l_addr)+strlen(l_line), "%s", l_ascii);
                        }
                }
                else {
                        strncat(l_line, " ", _LINE_SIZE);
                        strncat(l_line, l_ascii, _LINE_SIZE);
                        NDBG_OUTPUT("%s\n", l_line);
                }
                ++l_line_num;
        }
        if (g_curses &&
            a_win) {
                wrefresh(a_win);
        }
}
#endif
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static void _sas_to_str(const struct sockaddr_storage* a_sas,
                        char* ao_buf)
{
        char l_addr_tmp[40];
        uint16_t l_port = 0;
        if (a_sas->ss_family == AF_INET)
        {
                struct sockaddr_in* l_sin = (struct sockaddr_in*)a_sas;
                inet_ntop(AF_INET, &l_sin->sin_addr, l_addr_tmp, sizeof(l_addr_tmp));
                l_port = ntohs(l_sin->sin_port);
                snprintf(ao_buf, 64, "%s:%u", l_addr_tmp, l_port);
        }
        else if(a_sas->ss_family == AF_INET6)
        {
                struct sockaddr_in6* l_sin6 = (struct sockaddr_in6*)a_sas;
                inet_ntop(AF_INET6, &l_sin6->sin6_addr, l_addr_tmp, sizeof(l_addr_tmp));
                l_port = ntohs(l_sin6->sin6_port);
                snprintf(ao_buf, 64, "[%s]:%u", l_addr_tmp, l_port);
        }
}
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
        // splice pipe
        // -------------------------------------------------
        errno = 0;
        l_s = pipe(a_client->m_pipe_fd);
        if (l_s < 0) {
                NDBG_ABORT("Error performing pipe. Reason: %s\n",
                           strerror(errno));
        }
        // -------------------------------------------------
        // read pipe
        // -------------------------------------------------
        errno = 0;
        l_s = pipe(a_client->m_pipe_rd_fd);
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
        _SET_NONBLOCK(a_client->m_pipe_fd[0]);
        _SET_NONBLOCK(a_client->m_pipe_fd[1]);
        _SET_NONBLOCK(a_client->m_pipe_rd_fd[0]);
        _SET_NONBLOCK(a_client->m_pipe_rd_fd[1]);
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _proxy_connect(client_t* a_client)
{
        // -------------------------------------------------
        // proxy connect (non-blocking)
        // -------------------------------------------------
        int l_s;
        errno = 0;
        socklen_t l_slen = sizeof(struct sockaddr_in);
        if (a_client->m_proxy_sas.ss_family == AF_INET6)
        {
                l_slen = sizeof(struct sockaddr_in6);
        }
#define _BLOCKING_CONNECT 1
#ifdef _BLOCKING_CONNECT
        while (1) {
#endif
        l_s = connect(a_client->m_proxy_fd,
                      (struct sockaddr*)(&(a_client->m_proxy_sas)),
                      l_slen);
        if (l_s < 0) {
                if ((errno == EINPROGRESS) ||
                    (errno == EALREADY) ||
                    (errno == EAGAIN)) {
#ifdef _BLOCKING_CONNECT
                        usleep(10000);
                        continue;
#else
                        return TS_STATUS_OK;
#endif
                }
                NDBG_ABORT("Error performing connect. Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
#ifdef _BLOCKING_CONNECT
        break;
        }
#endif
        a_client->m_socks_state = SOCKS_STATE_CONNECTED;
        // -------------------------------------------------
        // send back to client proxy connected
        // -------------------------------------------------
        // 4 + 18
        uint8_t l_resp[4] = { 0x05, 0x00, 0x00, 0x01 };
        if (a_client->m_proxy_sas.ss_family == AF_INET6) {
                l_resp[3] = 0x04;
        }
        uint8_t l_buf[22];
        memcpy(l_buf, l_resp, 4);
        memcpy(l_buf+4, a_client->m_client_connect_buf, a_client->m_client_connect_buf_len);
        l_s = send(a_client->m_client_fd, l_buf, 4+a_client->m_client_connect_buf_len, 0);
        if (l_s < 0) {
                return TS_STATUS_ERROR;
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _client_teardown(client_t* a_client, fd_set* a_rd_fdset, fd_set* a_wr_fdset)
{
        if (a_client->m_client_fd >= 0) {
                if (a_rd_fdset) {
                        FD_CLR(a_client->m_client_fd, a_rd_fdset);
                }
                if (a_wr_fdset) {
                        FD_CLR(a_client->m_client_fd, a_wr_fdset);
                }
                close(a_client->m_client_fd);
                a_client->m_client_fd = -1;
        }
        if (a_client->m_proxy_fd >= 0) {
                if (a_rd_fdset) {
                        FD_CLR(a_client->m_proxy_fd, a_rd_fdset);
                }
                if (a_wr_fdset) {
                        FD_CLR(a_client->m_proxy_fd, a_wr_fdset);
                }
                close(a_client->m_proxy_fd);
                a_client->m_proxy_fd = -1;
        }
        a_client->m_socks_state = SOCKS_STATE_NONE;
        int i_p = 0;
        for (i_p = 0; i_p < 2; ++i_p) {
                if (a_client->m_pipe_fd[i_p] > 0) {
                        close(a_client->m_pipe_fd[i_p]);
                        a_client->m_pipe_fd[i_p] = -1;
                }
        }
        for (i_p = 0; i_p < 2; ++i_p) {
                if (a_client->m_pipe_rd_fd[i_p] > 0) {
                        close(a_client->m_pipe_rd_fd[i_p]);
                        a_client->m_pipe_rd_fd[i_p] = -1;
                }
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _client_socks_readable(client_t* a_client, int a_tee)
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
                                  a_client->m_pipe_rd_fd[1],
                                  _BUF_SIZE,
                                  SPLICE_F_NONBLOCK);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        break;
                                }
                                NDBG_ABORT("Error performing tee[%d]. Reason[%d]: %s\n",
                                           l_s, errno, strerror(errno));
                        }
                        // ---------------------------------
                        // read 16 bytes at a time
                        // ---------------------------------
                        size_t l_off = 0;
                        do {
                                uint8_t l_buf[16];
                                l_s = read(a_client->m_pipe_rd_fd[0], l_buf, 16);
                                if (l_s > 0) {
#ifdef _USE_NCURSES
                                        if (g_curses) {
                                                _mem_display_win((const uint8_t*)l_buf, (size_t)l_s, (size_t)l_off, g_win_rqst);
                                        }
                                        else {
                                                NDBG_HEXDUMP(l_buf, l_s, l_off);
                                        }
#else
                                        NDBG_HEXDUMP(l_buf, l_s, l_off);
#endif
                                        l_off += l_s;
                                }
                        } while (l_s > 0);
                }
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
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _addr_to_sockaddr(struct sockaddr_storage* a_sas,
                             int a_type,
                             char* a_buf,
                             size_t a_len)
{
        // -------------------------------------------------
        // IPv4
        // -------------------------------------------------
        if (a_type == 0x01) {
                uint16_t l_port;
                memcpy(&l_port, a_buf+4, sizeof(l_port));
                struct sockaddr_in* l_sin = (struct sockaddr_in*)a_sas;
                memcpy(&(l_sin->sin_addr.s_addr), a_buf, 4);
                l_sin->sin_port = l_port;
                l_sin->sin_family = AF_INET;
        }
        // -------------------------------------------------
        // IPv6
        // -------------------------------------------------
        else if (a_type == 0x04) {
                uint16_t l_port;
                memcpy(&l_port, a_buf+16, sizeof(l_port));
                struct sockaddr_in6* l_sin6 = (struct sockaddr_in6*)a_sas;
                memcpy(&(l_sin6->sin6_addr), a_buf, 16);
                l_sin6->sin6_port = l_port;
                l_sin6->sin6_family = AF_INET6;
        }
        else {
                return TS_STATUS_ERROR;
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _client_readable(client_t* a_client,
                            fd_set* a_rd_fdset,
                            int a_tee)
{
        int l_s;
        // -------------------------------------------------
        // for socks state...
        // -------------------------------------------------
        switch(a_client->m_socks_state) {
        // -------------------------------------------------
        // SOCKS_STATE_NONE
        // -------------------------------------------------
        case SOCKS_STATE_NONE: {
                while (1) {
                        char l_buf[256];
                        l_s = recv(a_client->m_client_fd, l_buf, 256, 0);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        return TS_STATUS_OK;
                                }
                                return TS_STATUS_ERROR;
                        }
                        // EOF
                        else if (l_s == 0) {
                                return TS_STATUS_DONE;
                        }
                        uint8_t l_resp[2] = { 0x5, 0x0};
                        l_s = send(a_client->m_client_fd, l_resp, 2, 0);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        return TS_STATUS_OK;
                                }
                                return TS_STATUS_ERROR;
                        }
                        // EOF
                        else if (l_s == 0) {
                                return TS_STATUS_DONE;
                        }
                        a_client->m_socks_state = SOCKS_STATE_CLIENT_CONNECT;
                        break;
                }
                break;
        }
        // -------------------------------------------------
        // SOCKS_STATE_CLIENT_CONNECT
        // -------------------------------------------------
        case SOCKS_STATE_CLIENT_CONNECT: {
                // -----------------------------------------
                // proxy host spec:
                // -----------------------------------------
                // - 0x5, 0x1, 0x0, 0xN,
                // - byte 2:
                //     0x01: establish TCP/IP stream conn
                //     0x02: establish TCP/IP port binding
                //     0x03: establish UDP port
                // - byte 3: reserved 0/1
                // - byte 4:
                //     0x01: IPv4 address (4 byte payload)
                //     0x03: Domain name (len prefix <N bytes><name N chars>
                //     0x04: IPv6 address (16 byte payload)
                // - <HOST ADDR>, <HOST PORT>
                // -----------------------------------------
                while (1) {
                        char l_buf[256];
                        l_s = recv(a_client->m_client_fd, l_buf, 256, 0);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        return TS_STATUS_OK;
                                }
                                return TS_STATUS_ERROR;
                        }
                        // EOF
                        else if (l_s == 0) {
                                return TS_STATUS_DONE;
                        }
                        else if (l_s < 4) {
                                return TS_STATUS_ERROR;
                        }
                        // ---------------------------------
                        // copy in
                        // ---------------------------------
                        size_t l_len = (l_s-4) > 18 ? 18 : (l_s-4);
                        memcpy(a_client->m_client_connect_buf, l_buf+4, l_len);
                        a_client->m_client_connect_buf_len = l_len;
                        // ---------------------------------
                        // parse ip/port
                        // ---------------------------------
                        l_s = _addr_to_sockaddr(&(a_client->m_proxy_sas),
                                                (int)l_buf[3],
                                                (char*)a_client->m_client_connect_buf,
                                                a_client->m_client_connect_buf_len);
                        if (l_s != TS_STATUS_OK) {
                                return TS_STATUS_ERROR;
                        }
                        // ---------------------------------
                        // proxy socket
                        // ---------------------------------
                        errno = 0;
                        a_client->m_proxy_fd = socket(a_client->m_proxy_sas.ss_family, SOCK_STREAM, 0);
                        if (a_client->m_proxy_fd < 0) {
                                NDBG_ABORT("Error performing socket. Reason: %s\n",
                                           strerror(errno));
                        }
                        // ---------------------------------
                        // set flags
                        // ---------------------------------
                        int l_flags;
                        _SET_NONBLOCK(a_client->m_proxy_fd);
                        FD_SET(a_client->m_proxy_fd, a_rd_fdset);
                        // ---------------------------------
                        // start upstream connection
                        // ---------------------------------
                        a_client->m_socks_state = SOCKS_STATE_UPSTREAM_CONNECT;
                        l_s = _proxy_connect(a_client);
                        if (l_s != TS_STATUS_OK) {
                                return TS_STATUS_ERROR;
                        }
                        break;
                }
                break;
        }
        // -------------------------------------------------
        // SOCKS_STATE_CLIENT_CONNECT
        // -------------------------------------------------
        case SOCKS_STATE_UPSTREAM_CONNECT: {
                break;
        }
        // -------------------------------------------------
        // SOCKS_STATE_CLIENT_CONNECT
        // -------------------------------------------------
        case SOCKS_STATE_CONNECTED: {
                return _client_socks_readable(a_client, a_tee);
        }
        // -------------------------------------------------
        // default
        // -------------------------------------------------
        default: {
                break;
        }
        }
        return TS_STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _proxy_socks_readable(client_t* a_client, int a_tee)
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
                                  a_client->m_pipe_rd_fd[1],
                                  _BUF_SIZE,
                                  SPLICE_F_NONBLOCK);
                        if (l_s < 0) {
                                if (errno == EAGAIN) {
                                        break;
                                }
                                NDBG_ABORT("Error performing tee[%d]. Reason[%d]: %s\n",
                                           l_s, errno, strerror(errno));
                        }
                        // ---------------------------------
                        // read 16 bytes at a time
                        // ---------------------------------
                        size_t l_off = 0;
                        do {
                                uint8_t l_buf[16];
                                l_s = read(a_client->m_pipe_rd_fd[0], l_buf, 16);
                                if (l_s > 0) {
#ifdef _USE_NCURSES
                                        if (g_curses) {
                                                _mem_display_win((const uint8_t*)l_buf, (size_t)l_s, (size_t)l_off, g_win_resp);
                                        }
                                        else {
                                                NDBG_HEXDUMP(l_buf, l_s, l_off);
                                        }
#else
                                        NDBG_HEXDUMP(l_buf, l_s, l_off);
#endif
                                        l_off += l_s;
                                }
                        } while (l_s > 0);
                }
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
        }
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
        // for socks state...
        // -------------------------------------------------
        switch(a_client->m_socks_state) {
        // -------------------------------------------------
        // SOCKS_STATE_UPSTREAM_CONNECT
        // -------------------------------------------------
        case SOCKS_STATE_UPSTREAM_CONNECT: {
                l_s = _proxy_connect(a_client);
                if (l_s != TS_STATUS_OK) {
                        return TS_STATUS_ERROR;
                }
                break;
        }
        // -------------------------------------------------
        // SOCKS_STATE_CLIENT_CONNECT
        // -------------------------------------------------
        case SOCKS_STATE_CONNECTED: {
                return _proxy_socks_readable(a_client, a_tee);
        }
        // -------------------------------------------------
        // default
        // -------------------------------------------------
        default: {
                break;
        }
        }
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
        fprintf(a_stream, "  -c, --curses  curses display\n");
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
        int l_curses = 0;
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
                { "curses", no_argument,       0, 'c' },
               // Sentinel
                { 0,        0,                 0,  0  }
        };
        // -------------------------------------------------
        // args...
        // -------------------------------------------------
        const char l_short_arg_list[] = "hl:p:tc";
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
                case 'c': {
                        g_curses = 1;
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
        // curses view
        // -------------------------------------------------
#ifdef _USE_NCURSES
        if (g_curses)
        {
                signal(SIGWINCH, _handle_winch);
                initscr();
                // -----------------------------------------
                // colors
                // -----------------------------------------
                // TODO check for has colors?
                //if (has_colors() == FALSE) { ... }
                start_color();
                init_pair(1, COLOR_BLUE, COLOR_BLACK);
                init_pair(2, COLOR_GREEN, COLOR_BLACK);
                init_pair(3, COLOR_WHITE, COLOR_BLACK);
                init_pair(4, COLOR_YELLOW, COLOR_BLACK);
                init_pair(5, COLOR_CYAN, COLOR_BLACK);
                // -----------------------------------------
                // windows
                // -----------------------------------------
                struct winsize l_ws;
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &l_ws);
                g_term_ws_row = l_ws.ws_row;
                g_term_ws_col = l_ws.ws_col;
                // creating a window;
                g_win_rqst = newwin(l_ws.ws_row, l_ws.ws_col/2, 0, 0);
                g_win_resp = newwin(l_ws.ws_row, l_ws.ws_col/2, 0, l_ws.ws_col/2);
                refresh();
                // making box border with default border styles
                wattron(g_win_rqst, COLOR_PAIR(5));
                box(g_win_rqst, 0, 0);
                wattron(g_win_resp, COLOR_PAIR(4));
                box(g_win_resp, 0, 0);
                // move and print in window
                mvwprintw(g_win_rqst, 0, 1, "Request");
                mvwprintw(g_win_resp, 0, 1, "Response");
                // refreshing the window
                wrefresh(g_win_rqst);
                wrefresh(g_win_resp);
        }
#endif
        int l_s;
        // -------------------------------------------------
        // if tee ensure stdout is pipe
        // -------------------------------------------------
#if 0
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
#endif
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
#if 0
        errno = 0;
        l_s = setsockopt(l_listen_fd, SOL_SOCKET, TCP_NODELAY, &l_val, sizeof(l_val));
        if (l_s < 0) {
                NDBG_ABORT("Error setsockopt() Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
#endif
        // -------------------------------------------------
        // set zero copy
        // requires setcap ?
        // -------------------------------------------------
#if 0
        errno = 0;
        l_s = setsockopt(l_listen_fd, SOL_SOCKET, SO_ZEROCOPY, &l_val, sizeof(l_val));
        if (l_s < 0) {
                NDBG_ABORT("Error setsockopt() Reason[%d]: %s\n",
                           errno, strerror(errno));
        }
#endif
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
        FD_ZERO(&l_rd_fdset);
        FD_SET(l_listen_fd, &l_rd_fdset);
        // -------------------------------------------------
        // client
        // -------------------------------------------------
        client_t l_client;
        l_client.m_socks_state = SOCKS_STATE_NONE;
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
                // ----------------------------------------
                // set max fd
                // ----------------------------------------
                if (l_client.m_client_fd > l_max_fd) {
                        l_max_fd = l_client.m_client_fd;
                }
                if (l_client.m_proxy_fd > l_max_fd) {
                        l_max_fd = l_client.m_proxy_fd;
                }
                // ----------------------------------------
                // select on fd
                // ----------------------------------------
                errno = 0;
                l_s = select(l_max_fd+1,
                             &l_tmp_rd_fdset,
                             NULL,
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
                                l_s = _client_teardown(&l_client, &l_rd_fdset, NULL);
                                UNUSED(l_s);
                                l_max_fd = l_listen_fd + 1;
                                continue;
                        }
                        // ---------------------------------
                        // set flags
                        // ---------------------------------
                        FD_SET(l_client.m_client_fd, &l_rd_fdset);
#ifdef _USE_NCURSES
                        // ---------------------------------
                        // clear windows
                        // ---------------------------------
                        if (g_curses) {
                                _clear_display_win(g_win_rqst);
                                _clear_display_win(g_win_resp);
                        }
#endif
                }
#define _TEARDOWN_CHECK(_s) do { \
                if (_s != TS_STATUS_OK) { \
                        int _l_s = _client_teardown(&l_client, &l_rd_fdset, NULL); \
                        UNUSED(_l_s); \
                        l_max_fd = l_listen_fd + 1; \
                        continue; \
                } } while(0)
                // ----------------------------------------
                // client readable
                // ----------------------------------------
                if(FD_ISSET(l_client.m_client_fd, &l_tmp_rd_fdset)) {
                        l_s = _client_readable(&l_client, &l_rd_fdset, l_tee);
                        _TEARDOWN_CHECK(l_s);
                }
                // ----------------------------------------
                // proxy readable
                // ----------------------------------------
                if(FD_ISSET(l_client.m_proxy_fd, &l_tmp_rd_fdset)) {
                        l_s = _proxy_readable(&l_client, l_tee);
                        _TEARDOWN_CHECK(l_s);
                }
        }
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        if (l_listen_fd > 0) { close(l_listen_fd); l_listen_fd = -1; }
#ifdef _USE_NCURSES
        if (g_curses)
        {
                getch();
                endwin();
        }
#endif
        return TS_STATUS_OK;
}
