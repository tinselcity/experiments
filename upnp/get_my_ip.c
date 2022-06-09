//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
// ---------------------------------------------------------
// this app
// ---------------------------------------------------------
#include "upnp.h"
// ---------------------------------------------------------
// std includes
// ---------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
//! ----------------------------------------------------------------------------
//! typedefs
//! ----------------------------------------------------------------------------
typedef unsigned char uchar_t;
//! ----------------------------------------------------------------------------
//! \details:  Get source address used for a given destination address.
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _get_source_address(struct sockaddr const *a_dst,
                               socklen_t a_dst_len,
                               struct sockaddr *a_src,
                               socklen_t *a_src_len)
{
        // -------------------------------------------------
        // make socket
        // -------------------------------------------------
        int l_fd;
        l_fd = socket(a_dst->sa_family, SOCK_DGRAM, 0);
        if (l_fd == -1)
        {
                NDBG_OUTPUT(": error performing socket. Reason: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // connect
        // UDP socket doesn't send packets to connect
        // -------------------------------------------------
        int l_s;
        l_s = connect(l_fd, a_dst, a_dst_len);
        if (l_s != 0)
        {
                NDBG_OUTPUT(": error performing connect. Reason: %s\n", strerror(errno));
                if (l_fd) { close(l_fd); l_fd = -1; }
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // get sockname
        // -------------------------------------------------
        l_s = getsockname(l_fd, a_src, a_src_len);
        if (l_s != 0)
        {
                NDBG_OUTPUT(": error performing getsockname. Reason: %s\n", strerror(errno));
                if (l_fd) { close(l_fd); l_fd = -1; }
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // done
        // -------------------------------------------------
        close(l_fd);
        return STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _get_public_address_v4(uint32_t *a_addr)
{
        int l_s;
        struct sockaddr_storage l_sas;
        socklen_t l_sslen = sizeof(l_sas);
        struct sockaddr const *l_sa = NULL;
        socklen_t l_salen = 0;
        // -------------------------------------------------
        // make sockaddr_in
        // -------------------------------------------------
        struct sockaddr_in l_sin;
        memset(&l_sin, 0, sizeof(l_sin));
        l_sin.sin_family = AF_INET;
        // -------------------------------------------------
        // any real ipv4 address (not private)
        // -------------------------------------------------
        l_s = inet_pton(AF_INET, "8.8.8.8", &l_sin.sin_addr);
        if (l_s != 1)
        {
                NDBG_OUTPUT(": error performing inet_pton. Reason: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        l_sin.sin_port = htons(53);
        l_sa = (struct sockaddr const*) &l_sin;
        l_salen = sizeof(l_sin);
        // -------------------------------------------------
        // get source address
        // -------------------------------------------------
        l_s = _get_source_address(l_sa, l_salen, (struct sockaddr*) &l_sas, &l_sslen);
        if (l_s < 0)
        {
                NDBG_OUTPUT(": error performing get_source_address\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // check for is private
        // see: RFC1918
        // -------------------------------------------------
        uchar_t* l_c = NULL;
        l_c = (uchar_t*) &(((struct sockaddr_in*) &l_sas)->sin_addr);
        uchar_t l_c_0 = l_c[0];
        uchar_t l_c_1 = l_c[1];
        if (
            (l_c_0 == 0)   ||
            (l_c_0 == 127) ||
            (l_c_0 >= 224) ||
            (l_c_0 == 10)  ||
            ((l_c_0 == 172) &&
             (l_c_1 >= 16)  &&
             (l_c_1 <= 31)) ||
            ((l_c_0 == 192) &&
             (l_c_1 == 168))
            )
        {
                NDBG_OUTPUT(": warning ipv4 appears to be private address (RFC4193)\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // copy in
        // -------------------------------------------------
        memcpy(a_addr, &((struct sockaddr_in*) &l_sas)->sin_addr, 4);
        return STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static int _get_public_address_v6(void *a_addr)
{
        int l_s;
        struct sockaddr_storage l_sas;
        socklen_t l_sslen = sizeof(l_sas);
        struct sockaddr const *l_sa = NULL;
        socklen_t l_salen = 0;
        // -------------------------------------------------
        // make sockaddr_in6
        // -------------------------------------------------
        struct sockaddr_in6 l_sin6;
        memset(&l_sin6, 0, sizeof(l_sin6));
        l_sin6.sin6_family = AF_INET6;
        // -------------------------------------------------
        // any real ipv6 address (not private)
        // -------------------------------------------------
        l_s = inet_pton(AF_INET6, "2607:f8b0:4007:810::200e", &l_sin6.sin6_addr);
        if (l_s != 1)
        {
                NDBG_OUTPUT(": error performing inet_pton. Reason: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        l_sin6.sin6_port = htons(6969);
        l_sa = (struct sockaddr const*) &l_sin6;
        l_salen = sizeof(l_sin6);
        // -------------------------------------------------
        // get source address
        // -------------------------------------------------
        l_s = _get_source_address(l_sa, l_salen, (struct sockaddr*) &l_sas, &l_sslen);
        if (l_s < 0)
        {
                NDBG_OUTPUT(": error performing get_source_address\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // check for is private
        // see: RFC4193
        // -------------------------------------------------
        uchar_t* l_c = NULL;
        l_c = (uchar_t*) &((struct sockaddr_in6*) &l_sas)->sin6_addr;
        uchar_t l_c_0 = l_c[0];
        if ((l_c_0 & 0xE0) != 0x20)
        {
                NDBG_OUTPUT(": warning ipv6 appears to be private address (RFC4193)\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // copy in
        // -------------------------------------------------
        memcpy(a_addr, &((struct sockaddr_in6*) &l_sas)->sin6_addr, 16);
        return STATUS_OK;
}
//! ----------------------------------------------------------------------------
//! \details: display public address(es)
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
const char* get_public_address_v6_str(void)
{
        static char s_ipv6_str[INET6_ADDRSTRLEN] = "\0";
        if (strlen(s_ipv6_str) != 0)
        {
                return s_ipv6_str;
        }
        // -------------------------------------------------
        // ipv6
        // -------------------------------------------------
        int l_s;
        uchar_t l_ipv6_addr_str[INET_ADDRSTRLEN];
        l_s = _get_public_address_v6(l_ipv6_addr_str);
        if (l_s != STATUS_OK)
        {
                NDBG_OUTPUT(": error performing get_public_address\n");
                return NULL;
        }
        // -------------------------------------------------
        // convert to string
        // -------------------------------------------------
        const char* l_rs;
        errno = 0;
        l_rs = inet_ntop(AF_INET6, l_ipv6_addr_str, s_ipv6_str, INET6_ADDRSTRLEN);
        if (l_rs == NULL)
        {
                NDBG_OUTPUT(": error performing inet_ntop:  Reason: %s\n", strerror(errno));
                return NULL;
        }
        NDBG_OUTPUT(": ipv6: %s\n", s_ipv6_str);
        return s_ipv6_str;
}
//! ----------------------------------------------------------------------------
//! \details: display public address(es)
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
int32_t get_public_address(char* ao_ip_addr_str)
{
        NDBG_OUTPUT(": getting public ip address(es)\n");
        NDBG_OUTPUT(": trying IPv4...\n");
        // -------------------------------------------------
        // ipv4
        // -------------------------------------------------
        int l_s;
        uint32_t l_ipv4_addr;
        l_s = _get_public_address_v4(&l_ipv4_addr);
        if (l_s != STATUS_OK)
        {
                NDBG_OUTPUT(": warning: error performing get_public_address (IPv4)\n");
                goto get_ipv6;
        }
        // -------------------------------------------------
        // convert to string
        // -------------------------------------------------
        errno = 0;
        const char* l_rs;
        l_rs = inet_ntop(AF_INET, &l_ipv4_addr, ao_ip_addr_str, INET_ADDRSTRLEN);
        if (l_rs == NULL)
        {
                NDBG_OUTPUT(": error performing inet_ntop:  Reason: %s\n", strerror(errno));
                goto get_ipv6;
        }
        NDBG_OUTPUT(": ipv4: %s\n", ao_ip_addr_str);
get_ipv6:
        NDBG_OUTPUT(": trying IPv6...\n");
        // -------------------------------------------------
        // ipv6
        // -------------------------------------------------
        uchar_t l_ipv6_addr_str[INET_ADDRSTRLEN];
        l_s = _get_public_address_v6(l_ipv6_addr_str);
        if (l_s != STATUS_OK)
        {
                NDBG_OUTPUT(": warning: error performing get_public_address (IPv6)\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // convert to string
        // -------------------------------------------------
        errno = 0;
        l_rs = inet_ntop(AF_INET6, l_ipv6_addr_str, ao_ip_addr_str, INET6_ADDRSTRLEN);
        if (l_rs == NULL)
        {
                NDBG_OUTPUT(": error performing inet_ntop:  Reason: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        NDBG_OUTPUT(": ipv6: %s\n", ao_ip_addr_str);
        // -------------------------------------------------
        // done
        // -------------------------------------------------
        return STATUS_OK;
}
