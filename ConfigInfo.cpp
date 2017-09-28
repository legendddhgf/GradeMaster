#include "ConfigInfo.h"
#include "ExtraUtil.h"
#include <stdbool.h>

#define ENTRY_ERASED 1

// returns true if PARAM exists in an entry
bool ConfigInfo_s::entry_exists(char *param) {
  return params.find(string(param)) != params.end();
}

// Inserts/replaces an entry with key: param and value: value
// returns true if insertion/replacement was successful
bool ConfigInfo_s::entry_add(char *param, char *value) {
  if(entry_exists(param)) {
    // TODO: print a warning message
    params.erase(string(param)) == ENTRY_ERASED;
  }
  params.insert(pair<string, string>(string(param), string(value)));
  return entry_exists(param);
}

// returns true if the entry containing PARAM was successfully deleted
bool ConfigInfo_s::entry_remove(char *param) {
  return params.erase(string(param)) == ENTRY_ERASED;
}

ConfigInfo_s::ConfigInfo() {
  // nothing to do here?
}

ConfigInfo_s::ConfigInfo(FILE *fp) {
  if (!fp) {
    eexit("File pointer passed to ConfigInfo() is null\n");
  }
  char buf[256];
  uint16_t line_count = 0;
  while(fgets(buf, 255, fp)) { // break when finished with file
    line_count++;
    if (buf[0] == '\n') continue; // it's an empty line
    if (buf[strlen(buf) - 1] != '\n') {
      eexit("Line of configuration file too long:\n\"%s\"\n", buf);
    }
    char param[256];
    char value[256];
    char *tok = strtok(buf, ":");
    // TODO: should this just skip the line? I am leaning on keeping like this
    if (!tok) eexit("Line %d: Invalid format\n", line_count);
    sprintf(param, "%s", tok);
    tok = strtok(NULL, ":");
    if (!tok) eexit("Line %d: Invalid format\n", line_count);
    sprintf(value, "%s", tok);
    if (!entry_add(param, value)) {
      eexit("Line %d: Unable to add entry\n", line_count);
    }
  }
}

ConfigInfo_s::~ConfigInfo() {
  // nothing to do here?
}


