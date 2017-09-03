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
#include "StudentInfo.h"
#include "ConfigInfo.h"

#define TUPLE_SECTION_NAME 0
#define TUPLE_SECTION_MAXPTS 1
#define TUPLE_SECTION_PREDEFS 2

using namespace std;

// TODO: need a unique identifier? will there be a time where multiple
// assignments are loaded all at once?

// This struct will be constructed for the current assignment being worked on
// TODO: add storage for custom (pre-loaded) comments, special (pre-loaded)
// scores, etc
typedef struct AssignmentInfo {
  vector <GraderInfo_s *> asg_graders; // vector of pointers to info about a grader
  vector <StudentInfo_s *> asg_students; // vector of pointers to info about a student
  string asg_name; // string representing the name of the assignment
  uint16_t asg_maxpts; // a positive integer representing max possible pts
  // map of the hash of section information to the id of the section
  unordered_map <uint64_t, uint16_t> map_section_hash_to_id;
  // In the case of asg_sections, the index is the id used in
  // the "map_section_hash_to_id"
  // this contains name, maxpts, list of pre-defined comment/percentage pairs
  vector <tuple <string, uint16_t, vector <tuple<string, uint8_t>>>>
    asg_sections;
  AssignmentInfo(ConfigInfo *);
  ~AssignmentInfo(void);
  string getAsgName(void); // return stored string for assignment name
  uint16_t getAsgPts(void); // return stored integer for assignment maxpoints
  string getSectionName(uint16_t); // return stored string for section name
  uint16_t getSectionMaxPts(uint16_t); // return stored integer for section MaxPts
  uint16_t getSectionNumPredef(uint16_t); // return number of predef comments/%
  // returns the specified tuple containing the comment and grade for section
  tuple<string, uint8_t> getSectionPredefInfo(uint16_t, uint16_t);
} AssignmentInfo_s;

#endif

