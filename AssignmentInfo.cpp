#include "AssignmentInfo.h"

AssignmentInfo_s::AssignmentInfo(ConfigInfo_s *config) {
// TODO: first finish making the ConfigInfo struct
}

AssignmentInfo_s::~AssignmentInfo(void) {
// TODO can't destroy without knowing what to create
}

// return stored string for assignment name
string AssignmentInfo_s::getAsgName(void) {
  return asg_name;
}

// return stored integer for assignment maxpoints
uint16_t AssignmentInfo_s::getAsgPts(void) {
  return asg_maxpts;
}

// return stored string for section name
string AssignmentInfo_s::getSectionName(uint16_t secNum) {
  if (secNum >= asg_sections.size()) {
    fprintf(stderr, "Invalid section %u: There are only %lu sections\n",
        secNum, asg_sections.size()); // TODO: add debug print trace instead
    exit(1); // TODO: part of the debug print trace please
  }
  // return the section name portion of the tuple
  return get<TUPLE_SECTION_NAME>(asg_sections.at(secNum));
}

// return stored string for section name
uint16_t AssignmentInfo_s::getSectionMaxPts(uint16_t secNum) {
  if (secNum >= asg_sections.size()) {
    fprintf(stderr, "Invalid section %u: There are only %lu sections\n",
        secNum, asg_sections.size()); // TODO: add debug print trace instead
    exit(1); // TODO: part of the debug print trace please
  }
  // return the section max points portion of the tuple
  return get<TUPLE_SECTION_MAXPTS>(asg_sections.at(secNum));
}

// return number of predef comments/%
uint16_t AssignmentInfo_s::getSectionNumPredef(uint16_t secNum) {
  if (secNum >= asg_sections.size()) {
    fprintf(stderr, "Invalid section %u: There are only %lu sections\n",
        secNum, asg_sections.size()); // TODO: add debug print trace instead
    exit(1); // TODO: part of the debug print trace please
  }
  // return the number of predefined comment/percentage pairs from tuple
  return get<TUPLE_SECTION_PREDEFS>(asg_sections.at(secNum)).size();
}

// returns the specified tuple containing the comment and grade for section
tuple<string, uint8_t> AssignmentInfo_s::getSectionPredefInfo(uint16_t secNum,
    uint16_t predefNum) {
  if (secNum >= asg_sections.size()) {
    fprintf(stderr, "Invalid section %u: There are only %lu sections\n",
        secNum, asg_sections.size()); // TODO: add debug print trace instead
    exit(1); // TODO: part of the debug print trace please
  }
  // return the tuple containing the particular comment/% pair from main tuple
  return get<TUPLE_SECTION_PREDEFS>(asg_sections.at(secNum)).at(predefNum);
}

