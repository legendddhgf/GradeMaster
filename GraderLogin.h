#ifndef _GRADERLOGIN_H_
#define _GRADERLOGIN_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>

#include "BitVec.h"

using namespace std;

typedef struct GraderTask {
  
  unordered_map<string, BitVec *>; // the sections the grader is responsible for
} GraderTask_s

typedef struct GraderInfo {
  string name; // the name of the grader
  uint8_t id; // uniquely identify every grader for a given assignemnt (max 255)
  GraderTask_s task; // task struct for grader
} GraderInfo_s

#endif
