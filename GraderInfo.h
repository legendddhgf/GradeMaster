#ifndef _GRADERINFO_H_
#define _GRADERINFO_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>

#include "BitVec.h" // TODO: make this

using namespace std;

// each grader gets assigned a variable of type GraderInfo_s
typedef struct GraderInfo {
  string name; // the name of the grader
  uint8_t id; // uniquely identify every grader for a given assignemnt (max 255)
  BitVec_s *tasks; // 1 is set for every section this grader is responsible for
} GraderInfo_s

#endif
