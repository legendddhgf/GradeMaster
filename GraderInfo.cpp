#include "GraderInfo.h"

// create graderinfo instance for 'name' and sets initial capacity appropriately
GraderInfo_s::GraderInfo(string name_long, string name_short,
    uint16_t numSec) {

  this->name_long = name_long;
  this->name_short = name_short;
  this->secs = new BitVec((numSec - 1) / 8 + 1); // remember that this is in bytes
}

// delete all memory associated with the grader instance
GraderInfo_s::~GraderInfo(void) {
  delete this->secs;
}

// returns the long name stored for this grader
string GraderInfo_s::getNameLong(void) {
  return this->name_long;
}

// returns the short name stored for this grader
string GraderInfo_s::getNameShort(void) {
  return this->name_short;
}

// returns whether the grader responsible
bool GraderInfo_s::checkSection(uint16_t secIndex) {
  return this->secs->get(secIndex) == 1 ? true : false;
}

// allows specifcation of up to 64 sections
bool GraderInfo_s::specifySections(uint64_t mask) {
  return this->secs->maskSpec(mask) == ~1 ? false : true;
}
