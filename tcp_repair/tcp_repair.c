//: ----------------------------------------------------------------------------
//: includes
//: ----------------------------------------------------------------------------
#include <stdio.h>
#include <netinet/tcp.h>
//: ----------------------------------------------------------------------------
//: silently drop connection using TCP_REPAIR
//: ref: https://oroboro.com/dealing-with-network-port-abuse-in-sockets-in-c/
//: ----------------------------------------------------------------------------
int socket_freeze(void)
{
        // -------------------------------------------------
        // handle abuse...
        // -------------------------------------------------
        if(abuse)
        {
                // -----------------------------------------
                // read bytes to establish the connection
                // -----------------------------------------
                u32 tries = 20;
                while(tries)
                {
                        sleep(100);
                        char tmpBuf[32];
                        s32 readCount = recv(mSocket, &tmpBuf[0], 32, 0);
                        if (readCount > -1)
                        {
                                break;
                        }
                        tries--;
                }
                // -----------------------------------------
                // use TCP_REPAIR to "freeze" socket state
                // -----------------------------------------
#ifdef TCP_REPAIR
                int aux = 1;
                if(setsockopt(mSocket, SOL_TCP, TCP_REPAIR, &aux, sizeof( aux )) < 0)
                {
                        reportError("could not turn on repair mode");
                }
                // -----------------------------------------
                // no TCP REPAIR - abort close
                // -----------------------------------------
#else
                struct linger so_linger;
                so_linger.l_onoff = 1;
                so_linger.l_linger = 0;
                if(setsockopt(mSocket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof so_linger) < 0)
                {
                        reportError("Cannot turn off SO_LINGER");
                }
#endif // TCP_REPAIR
        }
        close(mSocket);
        return 0;
}
//: ----------------------------------------------------------------------------
//:
//: ----------------------------------------------------------------------------
int main(void)
{
        printf("hello world\n");
        return 0;
}
