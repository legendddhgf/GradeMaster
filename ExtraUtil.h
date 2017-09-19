#ifndef EXTRAUTIL_H_
#define EXTRAUTIL_H_

#include <stdio.h>
#include <stdlib.h>

// TODO: color the print out put red or something similar
#define EEXIT(...) { \ // print an error string and then exit with code -1
  fprintf(stderr, __VA_ARGS__); \
  exit(-1); \
}

#endif
