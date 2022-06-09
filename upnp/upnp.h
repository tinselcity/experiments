#ifndef _UPNP_H
#define _UPNP_H
//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdint.h>
//! ----------------------------------------------------------------------------
//! constants
//! ----------------------------------------------------------------------------
#ifndef _TRUE
#define _TRUE 1
#endif
#ifndef _FALSE
#define _FALSE 0
#endif
#ifndef STATUS_OK
#define STATUS_OK 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR -1
#endif
//! ----------------------------------------------------------------------------
//! debug macros
//! ----------------------------------------------------------------------------
#ifndef NDBG_OUTPUT
#define NDBG_OUTPUT(...) \
                do { \
                        fprintf(stdout, __VA_ARGS__); \
                        fflush(stdout); \
                } while(0)
#endif
#ifndef NDBG_PRINT
#define NDBG_PRINT(...) \
                do { \
                        fprintf(stdout, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
                        fprintf(stdout, __VA_ARGS__);               \
                        fflush(stdout); \
                } while(0)
#endif
//! ----------------------------------------------------------------------------
//! prototypes
//! ----------------------------------------------------------------------------
int32_t get_public_address(char* ao_ip_addr_str);
int32_t echo_server(uint16_t a_port);
#endif
