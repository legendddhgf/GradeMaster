#include "ExtraUtil.h"

// prints error to stderr and then exits program
// prints using red color
void eexit(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_RED);
  vfprintf(stderr, fmt, args);
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
  printf(ANSI_COLOR_CYAN);
  vprintf(fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
}

// returns the trimmed version of this string based off delimiters
string trimmed(string orig, string delimiters) {
  int begin = 0;
  int end = orig.length() - 1;
  while (begin < end) { // trim beginning
    if (delimiters.find(orig.at(begin)) == string::npos) {
      break;
    }
    begin++;
  }
  while (end > begin) { // trim at end
    if (delimiters.find(orig.at(end)) == string::npos) {
      break;
    }
    end++;
  }
  int length = end - begin + 1;
  return orig.substr(begin, length);
}
