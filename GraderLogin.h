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

#include "BitVec.h" // TODO: make this

using namespace std;

typedef struct GraderTask {
  uint8_t numSections;
  //TODO: should this be a vector or hashmap of bitvectors (one per assignment)
  BitVec_s *tasks; // sections grader is tasked with have that bit set
} GraderTask_s

typedef struct GraderInfo {
  string name; // the name of the grader
  uint8_t id; // uniquely identify every grader for a given assignemnt (max 255)
  GraderTask_s task; // task struct for grader
} GraderInfo_s

#endif
