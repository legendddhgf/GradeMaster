#ifndef _GRADERINFO_H_
#define _GRADERINFO_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>

#include "BitVec.h"

using namespace std;

// each grader gets assigned a variable of type GraderInfo_s holding information
// about their name, responsibilities and any other relevant details that may be
// TBD
typedef struct GraderInfo {
  string name_long; // the full name of the grader
  string name_short; // the shortened name of the grader
  BitVec_s *secs; // 1 is set for every section this grader is responsible for
  GraderInfo(string, string, uint16_t); // create graderinfo instance for 'name'
  ~GraderInfo(void); // delete all memory associated with the grader instance
  string getNameLong(void); // returns the long name stored for this grader
  string getNameShort(void); // returns the short name stored for this grader
  bool checkSection(uint16_t secIndex); // returns whether the grader responsible
  bool specifySections(uint64_t mask); // allows specifcation of up to 64 sections
} GraderInfo_s;

#endif
