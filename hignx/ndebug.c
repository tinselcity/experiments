#include "ndebug.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//! ----------------------------------------------------------------------------
//! \details: TODO
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void mem_display(const char* buf, size_t len) {
  char display_line[256] = "";
  unsigned int bytes_displayed = 0;
  char byte_display[8] = "";
  char ascii_display[17] = "";
  while (bytes_displayed < len) {
    unsigned int col = 0;
    snprintf(display_line,
             sizeof(display_line),
             "%s0x%08x %s",
             ANSI_COLOR_FG_BLUE, bytes_displayed, ANSI_COLOR_OFF);
    strcat(display_line, " ");
    strcat(display_line, ANSI_COLOR_FG_GREEN);
    while ((col < 16) &&
           (bytes_displayed < len)) {
      snprintf(byte_display,
               sizeof(byte_display),
               "%02x",
               (unsigned char)buf[bytes_displayed]);
      strcat(display_line, byte_display);
      if (isprint(buf[bytes_displayed])) {
        ascii_display[col] = buf[bytes_displayed];
      }
      else {
        ascii_display[col] = '.';
      }
      col++;
      bytes_displayed++;
      if (!(col % 4)) {
        strcat(display_line, " ");
      }
    }
    if ((col < 16) &&
        (bytes_displayed >= len)) {
      while (col < 16) {
        strcat(display_line, "..");
        ascii_display[col] = '.';
        col++;
        if (!(col % 4)) {
          strcat(display_line, " ");
        }
      }
    }
    ascii_display[col] = '\0';
    strcat(display_line, ANSI_COLOR_OFF);
    strcat(display_line, " ");
    strcat(display_line, ascii_display);
    NDBG_OUTPUT("%s\n", display_line);
  }
}
