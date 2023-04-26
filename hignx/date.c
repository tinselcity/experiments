//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include "date.h"
// Mach time support clock_get_time
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
//! ----------------------------------------------------------------------------
//! Constants
//! ----------------------------------------------------------------------------
#define MAX_TIMER_RESOLUTION_US 1000000
//! ----------------------------------------------------------------------------
//! global static
//! ----------------------------------------------------------------------------
static uint64_t g_cyles_us = 0;
uint64_t g_last_s = 0;
static char g_last_date_str[128];
// Date cache...
uint64_t g_last_date_str_s_rdtsc = 0;
uint64_t g_last_date_str_s = 0;
//! ----------------------------------------------------------------------------
//! \details: Get the rdtsc value
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static __inline__ uint64_t get_rdtsc64()
{
  uint32_t lo;
  uint32_t hi;
  // We cannot use "=A", since this would use %rax on x86_64
  __asm__ __volatile__("rdtsc"
                       : "=a"(lo), "=d"(hi));
  // output registers
  return (uint64_t)hi << 32 | lo;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
static inline int _use_cached_time(uint64_t *last_rdtsc)
{
  if (!g_cyles_us)
  {
    uint64_t l_start = get_rdtsc64();
    usleep(100000);
    g_cyles_us = (get_rdtsc64() - l_start) / 100000;
  }
  if ((get_rdtsc64() - *last_rdtsc) < MAX_TIMER_RESOLUTION_US * g_cyles_us)
  {
    return 1;
  }
  *last_rdtsc = get_rdtsc64();
  return 0;
}
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
const char *get_date_str(void) {
  if (_use_cached_time(&g_last_date_str_s_rdtsc) && g_last_s) {
    return g_last_date_str;
  }
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(g_last_date_str, sizeof g_last_date_str, "%a, %d %b %Y %H:%M:%S %Z", &tm);
  return g_last_date_str;
}
