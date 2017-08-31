#include "StudentInfo.h"

// create studentinfo instance for 'name' and sets init capacity appropriately
StudentInfo_s::StudentInfo(string name_long, string name_short,
    uint16_t numSec) {

  this->name_long = name_long;
  this->name_short = name_short;
  // make the student start as having each section ungraded
  for (uint16_t i = 0; i < numSec; i++) {
    this->grades.push_back(~0);
  }
}

// delete all memory associated with the student instance
StudentInfo_s::~StudentInfo(void) {
  // Nothing in the struct to dynamically delete yet
}

// returns the long name stored for this student
string StudentInfo_s::getNameLong(void) {
  return name_long;
}

// returns the short name stored for this student
string StudentInfo_s::getNameShort(void) {
  return name_short;
}

// returns whether the student responsible
uint8_t StudentInfo_s::checkSectionGrade(uint16_t secIndex) {
  return this->grades.at(secIndex);
}

// allows specifcation of up to 64 sections
uint8_t StudentInfo_s::setSectionGrade(uint16_t secIndex, uint8_t percentage) {
  return (this->grades.at(secIndex) = (percentage <= 100 ? percentage : ~0));
}
