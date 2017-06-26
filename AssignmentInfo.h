#ifndef _ASSIGNMENTINFO_H_
#define _ASSIGNMENTINFO_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>

#include "GraderInfo.h"

using namespace std;




// TODO: need a unique identifier? will there be a time where multiple
// assignments are loaded all at once?

// One of these will be constructed for the current assignment being worked on
// TODO: add storage for custom (pre-loaded) comments, special (pre-loaded)
// scores, etc
typedef struct AssignmentInfo {
  vector <GraderInfo_s *>; // vector of pointers to info about all graders
  string asg_str; // string representing the name of the assignment
  uint16_t asg_maxpts; // a positive integer representing max possible pts
  vector <tuple<string, uint16_t>> asg_sections; // name and maxpts for sections
} AssignmentInfo_s

#endif

