//: ----------------------------------------------------------------------------
//: includes
//: ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
//: ----------------------------------------------------------------------------
//: constants
//: ----------------------------------------------------------------------------
#define BUFFER_SIZE 1024
#ifndef STATUS_OK
#define STATUS_OK 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR -1
#endif
//: ----------------------------------------------------------------------------
//: macros
//: ----------------------------------------------------------------------------
#ifndef ERROR
#define ERROR(...) \
        do { \
                fprintf(stderr, "%s:%s.%d: error: ", __FILE__, __FUNCTION__, __LINE__); \
                fprintf(stderr, __VA_ARGS__);               \
                fflush(stderr); \
        } while(0)
#endif

//#define ERROR(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
//: ----------------------------------------------------------------------------
//: silently drop connection using TCP_REPAIR
//: ref: https://oroboro.com/dealing-with-network-port-abuse-in-sockets-in-c/
//: ----------------------------------------------------------------------------
static int tcp_repair_close(int a_fd)
{
        int l_s;
        (void)l_s;
        // -------------------------------------------------
        // use TCP_REPAIR to "freeze" socket state
        // -------------------------------------------------
#ifdef TCP_REPAIR
        int l_opt = 1;
        l_s = setsockopt(a_fd, SOL_TCP, TCP_REPAIR, &l_opt, sizeof(l_opt));
        if(l_s < 0)
        {
                ERROR("socket: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
#endif
        // -------------------------------------------------
        // std close
        // -------------------------------------------------
        close(a_fd);
        return STATUS_OK;
}
//: ----------------------------------------------------------------------------
//: run echo server
//: ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
        int l_s;
        // -------------------------------------------------
        // args
        // -------------------------------------------------
        if (argc < 3)
        {
                fprintf(stderr, "usage: %s (port) (freeze)\n", argv[0]);
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // get port...
        // -------------------------------------------------
        int l_port = 0;
        l_port = atoi(argv[1]);
        // -------------------------------------------------
        // get freeze...
        // -------------------------------------------------
        int l_freeze = 0;
        l_freeze = atoi(argv[2]);
        // -------------------------------------------------
        // create server socket
        // -------------------------------------------------
        int l_server_fd;
        l_server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(l_server_fd < 0)
        {
                ERROR("socket: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // create sockaddr_in
        // -------------------------------------------------
        struct sockaddr_in l_server_sockaddr;
        bzero( &l_server_sockaddr, sizeof(l_server_sockaddr));
        l_server_sockaddr.sin_family = AF_INET;
        l_server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        l_server_sockaddr.sin_port = htons(l_port);
        // -------------------------------------------------
        // set reuse...
        // -------------------------------------------------
        int l_opt = 1;
        setsockopt(l_server_fd, SOL_SOCKET, SO_REUSEADDR, &l_opt, sizeof(l_opt));
        // -------------------------------------------------
        // bind...
        // -------------------------------------------------
        l_s = bind(l_server_fd, (struct sockaddr *) &l_server_sockaddr, sizeof(l_server_sockaddr));
        if(l_s < 0)
        {
                ERROR("bind: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // listen...
        // -------------------------------------------------
        l_s = listen(l_server_fd, 16);
        if(l_s < 0)
        {
                ERROR("listen: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // accept loop
        // -------------------------------------------------
        while(1)
        {
                int l_clnt_fd;
                struct sockaddr_in l_clnt;
                socklen_t l_client_len = sizeof(l_clnt);
                // -----------------------------------------
                // client accept...
                // -----------------------------------------
                l_clnt_fd = accept(l_server_fd, (struct sockaddr *) &l_clnt, &l_client_len);
                if(l_clnt_fd < 0)
                {
                        ERROR("accept: %s\n", strerror(errno));
                        return STATUS_ERROR;
                }
                // -----------------------------------------
                // read loop
                // -----------------------------------------
                while(1)
                {
                        char l_buf[BUFFER_SIZE];
                        int l_read;
                        // ---------------------------------
                        // recv data...
                        // ---------------------------------
                        l_read = recv(l_clnt_fd, l_buf, BUFFER_SIZE, 0);
                        if(!l_read)
                        {
                                // done reading
                                break;
                        }
                        else if(l_read < 0)
                        {
                                ERROR("recv: %s\n", strerror(errno));
                                return STATUS_ERROR;
                        }
                        // ---------------------------------
                        // echo back
                        // ---------------------------------
                        l_s = send(l_clnt_fd, l_buf, l_read, 0);
                        if(l_s < 0)
                        {
                                ERROR("send: %s\n", strerror(errno));
                                return STATUS_ERROR;
                        }
                        // ---------------------------------
                        // shut down...
                        // ---------------------------------
                        if(l_freeze)
                        {
                                tcp_repair_close(l_clnt_fd);
                                l_clnt_fd = -1;
                                break;
                        }
                        else
                        {
                                close(l_clnt_fd);
                                l_clnt_fd = -1;
                                break;
                        }
                }
        }
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        close(l_server_fd);
        return STATUS_OK;
}
