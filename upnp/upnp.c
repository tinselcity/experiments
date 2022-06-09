//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
// ---------------------------------------------------------
// this app
// ---------------------------------------------------------
#include "upnp.h"
// ---------------------------------------------------------
// upnp
// ---------------------------------------------------------
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpdev.h>
#include <miniupnpc/upnpcommands.h>
// ---------------------------------------------------------
// std includes
// ---------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
typedef enum _upnp_igd_status
{
    UPNP_IGD_NONE = 0,
    UPNP_IGD_VALID_CONNECTED = 1,
    UPNP_IGD_VALID_NOT_CONNECTED = 2,
    UPNP_IGD_INVALID = 3
} _upnp_igd_status_t;
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int32_t upnp_setup(uint16_t a_port)
{
        NDBG_OUTPUT(": MINIUPNPC_API_VERSION: %d\n", MINIUPNPC_API_VERSION);
        // -------------------------------------------------
        // discover
        // -------------------------------------------------
        struct UPNPDev* l_dev = NULL;
        int l_err = UPNPDISCOVER_SUCCESS;
        errno = 0;
        NDBG_OUTPUT(": upnpDiscover: ...\n");
        l_dev = upnpDiscover(2000, // delay ms
                             NULL,
                             NULL,
                             0,
                             0,
                             2,
                             &l_err);
        NDBG_OUTPUT(": upnpDiscover: dev: %p\n", l_dev);
        NDBG_OUTPUT(": upnpDiscover: err: %d\n", l_err);
        if (l_dev == NULL)
        {
                NDBG_PRINT("error performing upnpDiscover. Reason[%d]: %s\n", errno, strerror(errno));
                return STATUS_ERROR;
        }
        NDBG_OUTPUT(": upnpDiscover: done...\n");
        // -------------------------------------------------
        // find internet gateway device
        // -------------------------------------------------
        int32_t l_s;
        struct UPNPUrls l_urls;
        struct IGDdatas l_datas;
        char l_lan_addr[16];
        NDBG_OUTPUT(": UPNP_GetValidIGD: ...\n");
        l_s = UPNP_GetValidIGD(l_dev, &l_urls, &l_datas, l_lan_addr, sizeof(l_lan_addr));
        if (l_s != UPNP_IGD_VALID_CONNECTED)
        {
                NDBG_PRINT("error performing UPNP_GetValidIGD. Reason[%d]: %s\n", errno, strerror(errno));
                NDBG_PRINT("If your router supports UPnP, please make sure UPnP is enabled.\n");
                freeUPNPDevlist(l_dev);
        }
        NDBG_OUTPUT(": UPNP_GetValidIGD: done...\n");
        NDBG_OUTPUT(": Found Internet Gateway Device: %s\n", l_urls.controlURL);
        NDBG_OUTPUT(": Local Address:                 %s\n", l_lan_addr);
        freeUPNPDevlist(l_dev);
        // -------------------------------------------------
        // create port string
        // -------------------------------------------------
        char l_port_str[16];
        snprintf(l_port_str, sizeof(l_port_str), "%u", a_port);
        // -------------------------------------------------
        // create desc string
        // -------------------------------------------------
        char l_desc[64];
        snprintf(l_desc, sizeof(l_desc), "%s at %d", "ntrnt", a_port);
        // -------------------------------------------------
        // add port mapping (TCP)
        // -------------------------------------------------
        errno = 0;
        NDBG_OUTPUT(": UPNP_AddPortMapping(TCP): port[%u]\n", a_port);
        l_s = UPNP_AddPortMapping(l_urls.controlURL,
                                  l_datas.first.servicetype,
                                  l_port_str,
                                  l_port_str,
                                  l_lan_addr,
                                  l_desc,
                                  "TCP",
                                  NULL,
                                  NULL);
        if (l_s != 0)
        {
                NDBG_PRINT("error performing UPNP_AddPortMapping (TCP). Reason[%d]: %s\n", errno, strerror(errno));
        }
        NDBG_OUTPUT(": UPNP_AddPortMapping(TCP): port[%u]: done...\n", a_port);
        // -------------------------------------------------
        // add port mapping (UDP)
        // -------------------------------------------------
        errno = 0;
        NDBG_OUTPUT(": UPNP_AddPortMapping(UDP): port[%u]\n", a_port);
        l_s = UPNP_AddPortMapping(l_urls.controlURL,
                                  l_datas.first.servicetype,
                                  l_port_str,
                                  l_port_str,
                                  l_lan_addr,
                                  l_desc,
                                  "UDP",
                                  NULL,
                                  NULL);
        if (l_s != 0)
        {
                NDBG_PRINT("error performing UPNP_AddPortMapping (UDP). Reason[%d]: %s\n", errno, strerror(errno));
        }
        NDBG_OUTPUT(": UPNP_AddPortMapping(UDP): port[%u]: done...\n", a_port);
#if 0
        // -------------------------------------------------
        // teardown
        // -------------------------------------------------
        l_s = UPNP_DeletePortMapping(l_urls.controlURL,
                                     l_datas.first.servicetype,
                                     l_port_str,
                                     "TCP",
                                     NULL);
        l_s = UPNP_DeletePortMapping(l_urls.controlURL,
                                     l_datas.first.servicetype,
                                     l_port_str,
                                     "UDP",
                                     NULL);
#endif
        return STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
        uint16_t l_port = 51413;
        // -------------------------------------------------
        // display public address
        // -------------------------------------------------
        char l_ip_str[INET6_ADDRSTRLEN];
        int32_t l_s;
        l_s = get_public_address(l_ip_str);
        if (l_s != STATUS_OK)
        {
                printf(": error performing display_public_address\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // run upnp -to punch hole for port
        // -------------------------------------------------
        l_s = upnp_setup(l_port);
        if (l_s != STATUS_OK)
        {
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // display address/port
        // -------------------------------------------------
        if (memchr(l_ip_str, ':', sizeof(l_ip_str)) != NULL)
        {
                NDBG_OUTPUT(": talk to me at\n:   [%s]:%u\n", l_ip_str, l_port);
        }
        else
        {
                NDBG_OUTPUT(": talk to me at\n:   %s:%u\n", l_ip_str, l_port);
        }
        // -------------------------------------------------
        // echo server
        // -------------------------------------------------
        l_s = echo_server(l_port);
        if (l_s != STATUS_OK)
        {
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        // TODO
        // -------------------------------------------------
        // done
        // -------------------------------------------------
        return 0;
}
