#ifndef EXTRAUTIL_H_
#define EXTRAUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>
#include <algorithm>
#include <string>

using namespace std;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void eexit(const char *fmt, ...);

void warn_printf(const char *fmt, ...);

void normal_printf(const char *fmt, ...);

// returns the trimmed version of this string based off delimiters
string trimmed(string orig, string delimiters);

#endif
