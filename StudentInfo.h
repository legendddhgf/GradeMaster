#ifndef _STUDENTINFO_H_
#define _STUDENTINFO_H_

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

// each student gets assigned a variable of type StudentInfo_s containing
// information about their name,their grades on all sections (ungraded by
// default), and any other relevant details that may be TBD
typedef struct StudentInfo {
  string name_long; // the full name of the student
  string name_short; // the shortened name of the student
  // NOTE that for the following: value of ~0 means section is ungraded
  // this is a vector of grade percentages
  vector <uint8_t> grades;
  StudentInfo(string, string, uint16_t); // create studentinfo instance
  ~StudentInfo(void); // delete all memory associated with the student instance
  string getNameLong(void); // returns the long name stored for this student
  string getNameShort(void); // returns the short name stored for this student
  uint8_t checkSectionGrade(uint16_t); // returns percentage for section
  // sets to percentage from 0-100 or 255 if invalid. Returns what was set.
  uint8_t setSectionGrade(uint16_t, uint8_t);
} StudentInfo_s;

#endif
