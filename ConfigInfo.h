#ifndef _CONFIGINFO_H_
#define _CONFIGINFO_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>

using namespace std;

// This struct will hold all of the info about the current run of the system
// such as the lcoations for various files and folders relevant to the system,
// the current grader / student being graded, etc. In general the details are
// TBD
typedef struct ConfigInfo {
  // TODO: how about directory paths?
  /* Get rid of this, everything will go in the hash map instead
  string asg_name;
  string instance_grader_name;
  uint16_t asg_maxpts; // a positive integer representing max possible pts
  vector <GraderInfo_s *> asg_graders; // vector of pointers to info about a grader
  vector <StudentInfo_s *> asg_students; // vector of pointers to info about a student
  // map of the hash of section information to the id of the section
  unordered_map <uint64_t, uint16_t> map_section_hash_to_id;
  // In the case of asg_sections, the index is the id used in
  // the "map_section_hash_to_id"
  // this contains name, maxpts, list of pre-defined comment/percentage pairs
  vector <tuple <string, uint16_t, vector <tuple<string, uint8_t>>>>
    asg_sections;
    */
  unordered_map <string, string> params; // map of all configuration parameters and their vals

  ConfigInfo(void);
  ConfigInfo(FILE *);
  ~ConfigInfo(void);
} ConfigInfo_s;

#endif

