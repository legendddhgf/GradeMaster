#include "ConfigInfo.h"
#include "ExtraUtil.h"

ConfigInfo_s::ConfigInfo() {
  // nothing to do here?
}

ConfigInfo_s::ConfigInfo(FILE *fp) {
  if (!fp) {
    EEXIT("File pointer passed to ConfigInfo() is null\n");
  }
  char buf[256];
  uint16_t line_count = 0;
  while(fgets(buf, 255, fp)) { // break when finished with file
    line_count++;
    if (buf[0] == '\n') continue; // it's an empty line
    if (buf[strlen(buf) - 1] != '\n') {
      EEXIT("Line of configuration file too long:\n\"%s\"\n", buf);
    }
    char param[256];
    char value[256];
    char *tok = strok(buf, ":");
    // TODO: should this just skip the line? I am leaning on keeping like this
    if (!tok) {
      EEXIT("Line %d: Invalid format\n");
    }
    sprintf(param, "%s", tok);
  }
}

ConfigInfo_s::~ConfigInfo() {
  // nothing to do here?
}


