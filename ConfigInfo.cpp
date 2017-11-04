#include "ConfigInfo.h"
#include "ExtraUtil.h"
#include <stdbool.h>

#define ENTRY_ERASED 1
#define CONFIG_INFO_BUFSIZE 1024

// returns true if PARAM exists in an entry
bool ConfigInfo_s::entry_exists(string param) {
  return params.find(param) != params.end();
}

// Inserts/replaces an entry with key: param and value: value
// returns true if insertion/replacement was successful
bool ConfigInfo_s::entry_add(string param, string value) {
  if(entry_exists(param)) {
    warn_printf("Overwriting entry %s to have value %s\n", param.c_str(),
        value.c_str());
    params.erase(param);
  }
  params.insert(pair<string, string>(param, value));
  return entry_exists(param);
}

// returns true if the entry containing PARAM was successfully deleted
bool ConfigInfo_s::entry_remove(string param) {
  return params.erase(param) == ENTRY_ERASED;
}

ConfigInfo_s::ConfigInfo() {
  // nothing to do here?
}

ConfigInfo_s::ConfigInfo(FILE *fp) {
  if (!fp) {
    eexit("File pointer passed to ConfigInfo() is null\n");
  }
  // TODO: do this entirely using strings and string.getline()
  char buf[CONFIG_INFO_BUFSIZE];
  uint16_t line_count = 0;
  while(fgets(buf, CONFIG_INFO_BUFSIZE - 1, fp)) {
    line_count++;
    if (buf[0] == '\n') continue; // skip empty lines
    if (strlen(buf) >= CONFIG_INFO_BUFSIZE - 1) {
      eexit("Line of configuration file too long:\n\"%s\"\n", buf);
    }
    // TODO: add comment capability
    string param;
    string value;
    char *tok = strtok(buf, ":");
    string tokstr;
    // If it's an invalid format at this point, user should fix this
    if (!tok) eexit("Line %d: Invalid format\n", line_count);
    tokstr = string(tok);
    param = trimmed(tokstr, string(" \r\n"));

    tok = strtok(NULL, ":");
    if (!tok) eexit("Line %d: Invalid format\n", line_count);
    tokstr = string(tok);
    value = trimmed(tokstr, string(" \r\n"));
    if (!entry_add(param, value)) {
      eexit("Line %d: Unable to add entry\n", line_count);
    }
  }
}

ConfigInfo_s::~ConfigInfo() {
  // nothing to do here?
}


