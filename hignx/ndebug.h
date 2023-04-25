#ifndef _NDEBUG_H
#define _NDEBUG_H
//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <stddef.h>
//! ----------------------------------------------------------------------------
//! constants
//! ----------------------------------------------------------------------------
#ifndef STATUS_OK
#define STATUS_OK 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR 0
#endif
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define FATAL(...) do { \
  fprintf(stderr, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  return STATUS_ERROR; \
  } while(0)
#define ERROR(...) do { \
  fprintf(stderr, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  } while(0)
#define TRACE(...) do { \
  fprintf(stdout, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
  fprintf(stdout, __VA_ARGS__); \
  } while(0)
//! ----------------------------------------------------------------------------
//! ANSI Color Code Strings
//!
//! Taken from:
//! http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html
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
  } while (0)
#endif
#ifndef NDBG_PRINT
#define NDBG_PRINT(...)do { \
    fprintf(stdout, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(stdout, __VA_ARGS__); \
    fflush(stdout); \
  } while (0)
#endif
//! ----------------------------------------------------------------------------
//! prototypes
//! ----------------------------------------------------------------------------
void mem_display(const char* buf, size_t len);
#endif