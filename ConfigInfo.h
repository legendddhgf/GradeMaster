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
  uint16_t PLACE_HOLDER; // TODO: what will the config have?
  ConfigInfo(void); // TODO: what is the minimum we can give it?
  ~ConfigInfo(void);
} ConfigInfo_s;

#endif

