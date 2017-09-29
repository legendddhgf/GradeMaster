#include "ExtraUtil.h"

// prints error to stderr and then exits program
// prints using red color
void eexit(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_RED);
  vprintf(fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
  exit(-1);
}

// prints using magenta color
void warn_printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_MAGENTA);
  vprintf(fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
}

// prints using blue color
void normal_printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_BLUE);
  vprintf(fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
}
