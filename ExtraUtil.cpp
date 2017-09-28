#include "ExtraUtil.h"

// prints error to stderr and then exits program
// TODO: make this print using a custom color
void eexit(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
