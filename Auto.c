/*
 * Auto.c
 * Main executable for the automatic grading utility
 */

#include "Auto.h"

#undef autoError(args...)
// autoError() alternative that includes stack trace
#define autoError(args...) {\
  printf("\a\x1b[31m");\
  autoPrint("ERROR resulting in program crash");\
  autoPrint(args);\
  printf("\x1b[0m");\
  debugPrint("INFO stack trace");\
  debugPrint("CLASS <%s>", classId);\
  debugPrint("ASG <%s>", asgId);\
  debugPrint("PATH <%s>", currentPath());\
  if (asgList) { \
    studentWrite(0);\
    autoWrite();\
  } else {\
    autoWarn("Stack trace didn't happen because list was not ready");\
  }\
  exit(1);\
}

#define commanded(arg) listGetCur(cmdList) && streq(listGetCur(cmdList), arg)

// Global vars
char cwd[STRLEN]; // Current working directory
char cmd[STRLEN * 2]; // Last read command
List* cmdList; // Last read command stack

char studentId[STRLEN]; // Current student Id
Table* studentTable; // Current student config

// Memory storage
Table* graderTable; // User config

char exeId[5] = "auto";
char exeName[10] = "automatic";
Table* exeTable;
char exeDir[STRLEN]; // Executable directory, called automatic
Table* helpTable; // Executable help strings
Table* macroTable; // Expandable command macros -xxx

char binDir[STRLEN]; // Bin directory in class folder

char classId[STRLEN]; // cmps012b-pt.s15
char classDir[STRLEN]; // Class directory

char asgId[STRLEN]; // pa1
char asgDir[STRLEN]; // Assignment submission directory. called pa1
char asgBinDir[STRLEN]; // Assignment bin directory. called bin/pa1

char configId[STRLEN];
char configDir[STRLEN];

Table* asgTable; // Assignment config
List* asgList; // List of all students
List* asgDupList;

char* keyName = "user.name";
char* keyId = "user.id";

int numSections; // number of different sections

// Temp vars
int tempInt = 0;
char tempString[STRLEN];
List* tempList;

int main(int argc, char **argv) {

  // Initialize program
  requireChangeDir("automatic_config"); // this is where the config will now be, #define maybe?
  exeTable = tableRead("main_config");
  if (!tableGet(exeTable, "exe.name")) tablePut(exeTable, "exe.name", "automatic");
  if (!tableGet(exeTable, "exe.id")) tablePut(exeTable, "exe.id", "auto");
  if (!tableGet(exeTable, "class.id")) autoError("You are required to specify a class ID as 'class.id'");
  if (!tableGet(exeTable, "class.dir")) autoError("You are reqired to specify a class directory as 'class.dir'");
  tablePut(exeTable, "config.id", "automatic_config"); // with future changes, we may allow it to be elsewhere
  tablePut(exeTable, "config.dir", currentPath());
  sprintf(configId, "%s", tableGet(exeTable, "config.id"));
  sprintf(configDir, "%s", tableGet(exeTable, "config.dir"));
  sprintf(classId, "%s", tableGet(exeTable, "class.id"));
  sprintf(classDir, "%s", tableGet(exeTable, "class.dir"));
  asgList = NULL;

  // Get arguments
  // TODO: Actually support arguments
  if(argc == 1) autoError("USAGE %s [flags] [class] assignment", exeId);
  if(argc >= 3 && argv[argc - 2][1] != "-") strcpy(classId, argv[argc - 2]);
  strcpy(asgId, argv[argc - 1]);
  if(streq(asgId, "bin"))
    autoError("ASG <%s> invalid", asgId);
  if (!tableGet(exeTable, "exe.dir")) {
    changeDir("..");
    tablePut(exeTable, "exe.dir", currentPath());
    requireChangeDir(configDir);
  }
  //tablePrint(exeTable, "%s: %s\n");
  //debugPrint("tableGet(exe.name) = %s", tableGet(exeTable, "exe.name"));

  // Get executable info
  helpTable = tableRead("help");
  macroTable = tableRead("macro");

  debugPrint("EXE <%s> loaded", tableGet(exeTable, "exe.dir"));

  // Get class info
  if(streq(classId, "")) {
    changeDir(".");
    chdir("/");
    strtok(cwd, "/");
    char* temp = cwd; // afs
    for(tempInt = 0; tempInt < 3; tempInt++) {
      // 0: cats.ucsc.edu
      // 1: class
      // 2: cmps012b-pt.s15
      chdir(temp);
      temp = strtok(NULL, "/");
      if(tempInt == 1 && strcmp(temp, "class"))
        autoError("CLASS not provided, implicitly or by argument");
    }
    strcpy(classId, temp);
    requireChangeDir(classId);
    strcpy(classDir, currentPath());
  } else {
    //requireChangeDir(classDir); // called anyway
  }
  autoPrint("CLASS <%s> loaded", classId);
  debugPrint("CLASS dir <%s> loaded", classDir);

  // Get bin info
  requireChangeDir(classDir);
  requireChangeDir("bin");
  strcpy(binDir, currentPath());
  debugPrint("BIN <%s> loaded", binDir);

  // Get asg info
  changeDir(classDir);
  requireChangeDir(asgId);
  autoPrint("ASG <%s> loaded", asgId);
  strcpy(asgDir, currentPath());

  // Get asgbin info
  changeDir(binDir);
  assertChangeDir(asgId);
  strcpy(asgBinDir, currentPath());
  //changeDir(asgBinDir);
  asgTable = tableRead(asgId);
  if (!tableSize(asgTable)) autoError("%s/%s.autotable Not Found", asgBinDir, asgId);
  {
    char numcon[4] = "1"; //this part is to force that entries for all part of assignment are added
    int num = 1;
    char tempstr[50];
    while (tableGet(asgTable, numcon)) {
      sprintf(tempstr, "%s.desc.1", numcon);
      if (!tableGet(asgTable, tempstr)) autoError("Invalid format in %s/%s.autotable", asgBinDir, asgId);
      sprintf(tempstr, "%s.maxpts", numcon);
      if (!tableGet(asgTable, tempstr)) autoError("Invalid format in %s/%s.autotable", asgBinDir, asgId);
      num++;
      sprintf(numcon, "%d", num);
    }
    numSections = num - 1;
  }
  debugPrint("ASG bin <%s> loaded", asgBinDir);

  // Get asglist
  changeDir(asgDir);
  asgList = dirList(asgId);
  if(listGetSize(asgList) <= 0)
    autoError("ASG <%s> has no submissions", asgId);

  // Get grader config
  changeDir(binDir);
  assertChangeDir(configDir);
  assertChangeDir("grader");
  sprintf(tempString, "grader_%s", getlogin());
  graderTable = tableRead(tempString);
  tablePut(graderTable, "user.id", getlogin());
  realName(tempString, getlogin());
  tablePut(graderTable, "user.name", tempString);
  autoPrint("GRADER <%s> (%s) loaded", tableGet(graderTable, "user.id"),
      tableGet(graderTable, "user.name"));
  tablePrint(graderTable, "%s: %s\n");

  // Run shell
  autoShell();

  // Free data
  autoWrite();
  return 0;
}

// Auto shell loop
// Assume start at root directory
void autoShell() {
  char *startDir = NULL; // will tell which directory we started on in this run of Auto
  listMoveFront(asgList);
  startDir = listGetCur(asgList);
  if (graderTable) { // attempt to seek to the last place the grader was at before they exited
    char temps[51];
    sprintf(temps, "LastDir_%s_%s", classId, asgId);
    // set startDir to the last directory, try to seek to that directory, if
    // it doesn't: warn grader to that effect
    if ((startDir = tableGet(graderTable, temps)) && !listSeek(asgList, startDir))
      autoWarn("The directory you were last at no longer exists");
  }
  studentRead();
  while(true) {
    autoPrompt();
    if (cmd[0] == '\0') {
      // do nothing
    } else if(commanded("exit")) {
      break;
    } else if (commanded("first")) {
      if (!listMoveFront(asgList)) {
        autoError("Couldn't move to first student <%s>",
            currentDir());
      }
      studentRead();
    } else if(commanded("next")) {
      if(! listMoveNext(asgList)) {
        listMoveFront(asgList);
        autoWarn("INFO end of list, moving to first student <%s>",
            currentDir());
      }
      studentRead();
    } else if(commanded("prev")) {
      if(! listMovePrev(asgList)) {
        listMoveBack(asgList);
        autoWarn("INFO beginning of list, moving to last student <%s>",
            currentDir());
      }
      studentRead();
    } else if (commanded("last")) {
      if (!listMoveBack(asgList)) {
        autoError("Couldn't move to last student <%s>",
            currentDir());
      }
      studentRead();
    } else if(0 && commanded("skip")) { // This seems to be a copy of previous
      if(! listMovePrev(asgList)) {
        listMoveBack(asgList);
        autoWarn("INFO beginning of list, moving to last student <%s>",
            currentDir());
      }
      studentRead();
    } else if(commanded("user")) {
      if(listMoveNext(cmdList)) {
        if(commanded("print")) {
          tablePrint(studentTable, "%s: %s\n");
        } else if(commanded("reset")) {
          studentRead();
        }
      } else {
        debugPrint("help user");
      }
    } else if (commanded("mail")) {
      sendMail();
      studentRead();
    } else if (commanded("grade")) {
      autoGrade();
      studentRead();
    } else if (commanded("compile")) {
      autoCompile();
      studentRead();
    } else if (commanded("cheaters")) {
      printf("List Cheaters by cruzid one by one (end with empty line):\n");
      char stemp[101];
      stemp[0] = '\0';
      while (stemp[0] != '\r') {
        fgets(stemp, 100, stdin);
        if (strlen(stemp) < 2) break;
        stemp[(strlen(stemp) - 1 <= 0 ? 0 : strlen(stemp) - 1)] = '\0'; // cut off the newline
        int cheated = (stemp[0] == '!' ? 0 : 1);
        char *stemp2 = (stemp[0] == '!' ? stemp + 1 : stemp);
        if (listContains(asgList, stemp2)) {
          listSeek(asgList, stemp2);
          studentRead();
          tablePut((studentTable), "cheated", (cheated ? "Y" : "N"));
          studentWrite(0);
          debugPrint("Student %s marked as %s\n", stemp2, cheated ? "Cheater" : "Not Cheater");
        } else {
          debugPrint("Student %s not found\n", stemp2);
        }
      }
      listMoveFront(asgList);
      studentRead();
    } else {
      /* listString doesn't work yet TODO
         listString(tempString, cmdList);
         system(tempString);
         */
      system(cmd);
    }
  }
}

void studentRead() {
  requireChangeDir(asgDir);
  requireChangeDir(listGetCur(asgList));
  strcpy(studentId, currentDir());
  requireChangeDir(asgBinDir);
  assertChangeDir("student");
  sprintf(tempString, "student_%s", studentId);
  if (studentTable) tableDestroy(studentTable);
  studentTable = tableRead(tempString);
  tablePut(studentTable, keyId, studentId);
  realName(tempString, studentId);
  tablePut(studentTable, keyName, tempString);
  requireChangeDir(asgDir);
  requireChangeDir(listGetCur(asgList));

  autoPrint("STUDENT <%s> (%s) loaded", studentId,
      tableGet(studentTable, keyName));
}

char *getTimeStr(char *str) {
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  sprintf(str, "%02d%02d%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); //hhmmss
  return str;
}

// safeToGrade tells you if it's safe to grade someone based off current value of studentId
// return 1 if safe or 0 otherwise
//
// something is safe to grade if a file hasn't been written for grading or readtime=writetime
// or it's been more than 10 minutes since being read
int safeToGrade() {
  char timestr[20];
  getTimeStr(timestr);
  char *readStr = tableGet(studentTable, "graded.lastRead");
  char *writeStr = tableGet(studentTable, "graded.lastWritten");
  if (!readStr) return 1; // safe if never read for grading
  if (writeStr && !strcmp(readStr, writeStr)) return 1; // written at same time to show that it's been graded before and is safe now
  int readSec = 3600 * (10 * readStr[0] + readStr[1]) +
    60 * (10 * readStr[2] + readStr[3]) + 10 * readStr[4] + readStr[5];
  int curSec = 3600 * (10 * timestr[0] + timestr[1]) +
    60 * (10 * timestr[2] + timestr[3]) + 10 * timestr[4] + timestr[5];
  if (readStr[0] == 2 && timestr[0] == 0) { // overflow occured
    curSec += 24 * 3600; // only curtime could possibly overflow
  }
  if (curSec - readSec > 10 * 60) return 1; // if the difference is more than 10 minutes, it's safe
  return 0;
}

// special student read when grading
int studentReadGrading() {
  requireChangeDir(asgDir);
  requireChangeDir(listGetCur(asgList));
  strcpy(studentId, currentDir());
  requireChangeDir(asgBinDir);
  assertChangeDir("student");
  sprintf(tempString, "student_%s", studentId);
  if (studentTable) tableDestroy(studentTable);
  studentTable = tableRead(tempString);
  tablePut(studentTable, keyId, studentId);
  realName(tempString, studentId);
  tablePut(studentTable, keyName, tempString);
  requireChangeDir(asgDir);
  requireChangeDir(listGetCur(asgList));

  char numcon[4] = "1"; //this part is to force that entries for all part of assignment are added
  int num = 1;
  char tempstr[50];
  int stuff_added = 0;
  while (tableGet(asgTable, numcon)) {
    sprintf(tempstr, "grade.%s", numcon);
    if (!tableGet(studentTable, tempstr)) {
      tablePut(studentTable, tempstr, "U");
      autoWarn("Adding default grade entry for section %d of %s to %s", num, asgId, studentId);
      stuff_added = 1;
    }
    sprintf(tempstr, "notes.%s", numcon);
    if (!tableGet(studentTable, tempstr)) {
      tablePut(studentTable, tempstr, "");
      autoWarn("Adding default notes entry for section %d of %s to %s", num, asgId, studentId);
      stuff_added = 1;
    }
    num++;
    sprintf(numcon, "%d", num);
  }
  if (stuff_added) {
    studentWrite(0);
    return studentReadGrading(); // need to reconstruct stuff after writing
  }
  if (!safeToGrade()) {
    autoWarn("Not yet safe to grade %s\n", studentId);
    return 0;
  }
  tablePut(studentTable, "graded.lastRead", getTimeStr(tempstr));
  studentWrite(0);
  studentRead();

  autoPrint("STUDENT <%s> (%s) loaded for grading", studentId,
      tableGet(studentTable, keyName));
  return 1;
}

void studentWrite(int changetime) {
  requireChangeDir(asgBinDir);
  assertChangeDir("student");
  if (changetime) {
    char tempstr[20];
    getTimeStr(tempstr);
    tablePut(studentTable, "graded.lastRead", tempstr); // overwrite both
    tablePut(studentTable, "graded.lastWritten", tempstr);
  }
  if(studentTable) tableWrite(studentTable);
  studentTable = NULL;
  requireChangeDir(asgDir);
  debugPrint("STUDENT <%s> closed", studentId);
  strcpy(studentId, "null");
}

// @param dir: directory to move to
// @return success
// Version of chdir() that also updates cwd string
bool changeDir(char* dir) {
  int ret = chdir(dir);
  getcwd(cwd, sizeof(cwd));
  return ret == 0;
}

// @param dir: directory to move to
// Version of changeDir() that creates dir if nonexistent
void assertChangeDir(char* dir) {
  if(! changeDir(dir)) {
    char temp[1024];
    sprintf(temp, "mkdir %s", dir);
    system(temp);
    changeDir(dir);
    autoWarn("DIR <%s> created", dir);
  }
}

// @ param dir: directory to move to
// Version of changeDir() that exits program if nonexistent
void requireChangeDir(char* dir) {
  if(! changeDir(dir)) {
    autoWarn("INFO could not find directory <%s>", dir);
    autoWarn("INFO listing directories in <%s>", currentDir());
    system("ls -d */");
    autoError("DIR <%s> not accessible", dir);
  }
}

// @return current directory path
char* currentPath() {
  getcwd(cwd, sizeof(cwd));
  return cwd;
}

// @return current directory name
// Warning: Uses strtok()
char* currentDir() {
  getcwd(cwd, sizeof(cwd));
  char* dir = strtok(cwd, "/");
  while(true) {
    char* temp = strtok(NULL, "/");
    if(! temp) return dir;
    dir = temp;
  }
}

void autoPrompt() {
  char temp[STRLEN * 2];
  if(cmdList) listDestroy(cmdList);
  cmdList = NULL;
  while(! cmdList) {
    autoInput(temp, "$");
    strcpy(cmd, temp);
    cmdList = listCreateFromToken(temp, " ");
  }
  listPrint(cmdList);
  listMoveFront(cmdList);
  if(listGetCur(cmdList) && listGetCur(cmdList)[0] == '-') {
    List *expandList = tableGetList(macroTable, &listGetCur(cmdList)[1], " ");
    if(expandList) {
      listRemove(cmdList, listGetCur(cmdList));
      listPrint(expandList);
      listPrint(cmdList);
      listConcat(expandList, cmdList);
      cmdList = expandList;
    } else {
      autoWarn("INFO could not expand macro <%s>", listGetCur(cmdList));
      autoPrompt();
    }
  }
  listMoveFront(cmdList);
}

// @param result: string to hold result of prompt
// Get input from user
void autoInput(char* result, char* prompt) {
  printf("[%s@%s %s]%s ", tableGet(graderTable, "user.id"), exeId, currentDir(),
      prompt);
  result[0] = '\0';
  fgets(result, 1023, stdin);
  if (strlen(result) < 1) {
    debugPrint("autoInput() detected newline");
    return;
  }

  result[strlen(result) - 1] = '\0';
}

// @return result
// Get boolean input from user
bool autoAsk(char *std) {
  char result[STRLEN * 2];
  sprintf(result, "(y/n)[%s]", std);
  autoInput(result, result);
  if(streq(result, "")) strcpy(result, std);

  return streq(result, "y")
    || streq(result, "Y")
    || streq(result, "yes")
    || streq(result, "Yes")
    || streq(result, "YES");
}

void autoConfigureResponsibilities() {
  char stemp[101];
  debugPrint("Here are the possible grading sections:");
  for (int i = 1; i <= numSections; i++) {
    sprintf(stemp, "%d", i);
    debugPrint("%d: %s", i, tableGet(asgTable, stemp));
    //sprintf(stemp, "%d.", i);
    //debugPrint("%s", tableGet(asgTable, stemp));
    sprintf(stemp, "%d.maxpts", i);
    debugPrint("\tMax Points: %s", tableGet(asgTable, stemp));
  }
  printf("Please enter the sections you are responsible for.\nOne number per line.\nEnd if line has no numbers.\n");
  List *responsibilityList = listCreate();
  while(1) {
    printf("Choose Responsibility: ");
    stemp[0] = '\0';
    fgets(stemp, 100, stdin);
    int start = -1;
    int stop = -1;
    for (int i = 0; i < strlen(stemp); i++) {
      if (isdigit(stemp[i])) {
        start = i;
        break;
      }
    }
    if (start == -1) break;
    for (int i = start + 1; i < strlen(stemp); i++) {
      if (!isdigit(stemp[i])) {
        stop = i + 1;
        break;
      }
    }
    if (stop == -1) stop = strlen(stemp);
    stemp[stop] = '\0';
    int newResponsibility = atoi(stemp + start);
    if (newResponsibility >= 1 && newResponsibility <= numSections) {
      sprintf(stemp, "%d", newResponsibility);
      listAppend(responsibilityList, stemp);
    } else {
      autoWarn("Section %d doesn't exist and will not be added to %s's responsibilities", newResponsibility, tableGet(graderTable, "user.id"));
    }
  }
  printf("Your responsibilities are sections:\n");
  if (listGetSize(responsibilityList)) listPrint(responsibilityList);
  else printf("No Responsibilities\n");
  sprintf(stemp, "Grading_%s_%s", classId, asgId);
  tablePutList(graderTable, stemp, responsibilityList, " ");
}

// Auto writeback on exit
void autoWrite() {
  requireChangeDir(binDir);
  requireChangeDir(configDir);
  if(exeTable) tableWrite(exeTable);
  if (graderTable) {
    requireChangeDir("grader");
    char temps[51];
    sprintf(temps, "LastDir_%s_%s", classId, asgId);
    tablePut(graderTable, temps, listGetCur(asgList));
    tableWrite(graderTable);
  }
  //requireChangeDir(asgBinDir);
  if(asgList) listDestroy(asgList);
  if(cmdList) listDestroy(cmdList);
  if(tempList) listDestroy(tempList);
  if(helpTable) tableDestroy(helpTable);
  if(macroTable) tableDestroy(macroTable);
  if(asgTable) tableDestroy(asgTable);
  debugPrint("EXE safely exited", exeId);
}

void autoGrade() {
  //printf("This method is not yet ready\n");
  //continue;
  int autocont = 0;
  if (listMoveNext(cmdList)) {
    if (commanded("all")) {
      autocont = 1;
    }
  }
  int count = -1;
  int wasSafeToGrade = 1;
  do {
    if (count < 0) {
      count = 0;
    } else {
      studentWrite(1);
      if(! listMoveNext(asgList)) {
        listMoveFront(asgList);
        autoWarn("INFO end of list, moving to first student <%s>",
            currentDir());
      }
      //studentRead();
    }
    char stemp[101];
    /* // we will have a different way of picking executables to run
    //printf("This function is not yet available\n");
    requireChangeDir(asgBinDir);
    //requireChangeDir(asgId);
    FILE *exe_test = fopen(asgId, "r");
    if (exe_test) {
    fclose(exe_test);
    sprintf(stemp, "cp %s/%s .", asgBinDir, asgId);
    system(stemp);
    sprintf(stemp, "./%s", asgId);
    system(stemp);
    sprintf(stemp, "rm -f %s", asgId);
    system(stemp);
    } else {
    autoWarn("The assignment grading script, %s, doesn't yet exist in %s/\n",
    asgId, asgBinDir);
    }
    */
    wasSafeToGrade = studentReadGrading();
    if (!wasSafeToGrade) continue;
    sprintf(stemp, "Grading_%s_%s", classId, asgId);
    if (!tableGet(graderTable, stemp)) {
      printf("\n");
      autoWarn("You have not yet defined your grading responsibilities for %s", asgId);
      autoConfigureResponsibilities();
    }
    char grade[numSections][5];
    char notes[numSections][4001];
    sprintf(stemp, "Grading_%s_%s", classId, asgId);
    List *responsibilityList = tableGetList(graderTable, stemp, " ");
    int skipCurrent = 1; // whether this student was already graded
    for (int i = 1; i <= numSections; i++) {
      sprintf(stemp, "grade.%d", i);
      sprintf(grade[i - 1], "%s", tableGet(studentTable, stemp));
      sprintf(stemp, "notes.%d", i);
      char newLineParse[4001]; // convert "\\n" to "\n"
      int tempPointer1 = 0;
      int tempPointer2 = 0;
      sprintf(newLineParse, "%s", tableGet(studentTable, stemp));
      while (tempPointer1 < strlen(newLineParse)) {
        if (newLineParse[tempPointer1] == '\\') {
          notes[i - 1][tempPointer2++] = newLineParse[++tempPointer1] == 'n' ? '\n' : newLineParse[tempPointer1];
          tempPointer1++;
        } else {
          notes[i - 1][tempPointer2++] = newLineParse[tempPointer1++];
        }
      }
      notes[i - 1][tempPointer2] = '\0';
      sprintf(stemp, "%d", i);
      if (listContains(responsibilityList, stemp) && streq(grade[i - 1], "U")) {
        skipCurrent = 0;
        //printf("Section %d is ungraded\n", i);
      }
    }
    if (!skipCurrent) { // run those executables
      for (int i = 1; i <= numSections; i++) {
        sprintf(stemp, "%d", i);
        if (!listContains(responsibilityList, stemp)) continue; // if not responsibility, don't bother
        sprintf(stemp, "grade.%d", i);
        if (strcmp(tableGet(studentTable, stemp), "U") != 0) continue; // if graded, don't bother
        sprintf(stemp, "%s_%d", asgId, i);
        char stemp2[201];
        requireChangeDir(asgBinDir);
        FILE *exe_test = fopen(stemp, "r");
        if (exe_test) {
          fclose(exe_test);
          sprintf(stemp2, "cp %s/%s %s/%s/%s", asgBinDir, stemp, asgDir, listGetCur(asgList), stemp);
          system(stemp2);
          requireChangeDir(asgDir);
          requireChangeDir(listGetCur(asgList));
          sprintf(stemp2, "./%s", stemp);
          system(stemp2);
          sprintf(stemp2, "rm -f %s", stemp);
          system(stemp2);
        }
      }
      requireChangeDir(asgDir);
      requireChangeDir(listGetCur(asgList));
    }
    for(;;) {
      if (skipCurrent) {
        printf("%s has already been graded\n", listGetCur(asgList));
        break;
      }
      printf("\nPlease enter a command (\"-h\" is help)\n\n[%s@autoGrade %s] ", tableGet(graderTable, "user.id"),
          listGetCur(asgList));
      fgets(stemp, 100, stdin);
      stemp[(strlen(stemp) - 1 <= 0 ? 0 : strlen(stemp) - 1)] = '\0'; // cut off the newline
      if (streq(stemp, "-h")) {
        printf("-h: help\n-w: write grade\n-wp: write only specified scores\n-s: skip temporarily\n");
        printf("-cgx: change grade for section x\n-cnx: change notes for section x\n");
        printf("-nr: new responsibilities\n-cr: check responsibilities\n");
        printf("-cs: check student's grade and comments (for sections you are responsible for only)\n");
        printf("-rs: run scripts for sections you are responsible for\n<anything else>: system(str)\n");
      } else if (streq(stemp, "-w")) {
        for (int i = 1; i <= numSections; i++) {
          sprintf(stemp, "%d", i);
          if (!listContains(responsibilityList, stemp)) continue;
          sprintf(stemp, "grade.%d", i);
          tablePut(studentTable, stemp, streq(grade[i - 1], "U") ? "P" : grade[i - 1]); // default to perfect scores
          sprintf(stemp, "notes.%d", i);
          char newLineParse[4001]; // convert "\\n" to "\n"
          int tempPointer1 = 0;
          int tempPointer2 = 0;
          while (tempPointer2 < strlen(notes[i - 1])) {
            if (notes[i - 1][tempPointer2] == '\n') {
              newLineParse[tempPointer1++] = '\\';
              newLineParse[tempPointer1++] = 'n';
              tempPointer2++;
            } else {
              newLineParse[tempPointer1++] = notes[i - 1][tempPointer2++];
            }
          }
          newLineParse[tempPointer1] = '\0';
          tablePut(studentTable, stemp, newLineParse);
        }
        printf("Now writing %s\n", listGetCur(asgList));
        break; // write to table
      } else if (streq(stemp, "-wp")) { // only writes specified grades (U's not updated to P's)
        for (int i = 1; i <= numSections; i++) {
          sprintf(stemp, "%d", i);
          if (!listContains(responsibilityList, stemp)) continue;
          sprintf(stemp, "grade.%d", i);
          tablePut(studentTable, stemp, grade[i - 1]); // default to perfect scores
          sprintf(stemp, "notes.%d", i);
          char newLineParse[4001]; // convert "\\n" to "\n"
          int tempPointer1 = 0;
          int tempPointer2 = 0;
          while (tempPointer2 < strlen(notes[i - 1])) {
            if (notes[i - 1][tempPointer2] == '\n') {
              newLineParse[tempPointer1++] = '\\';
              newLineParse[tempPointer1++] = 'n';
              tempPointer2++;
            } else {
              newLineParse[tempPointer1++] = notes[i - 1][tempPointer2++];
            }
          }
          newLineParse[tempPointer1] = '\0';
          tablePut(studentTable, stemp, newLineParse);
        }
        printf("Now writing %s\n", listGetCur(asgList));
        break; // write to table
      } else if (streq(stemp, "-s")) {
        printf("Now skipping %s\n", listGetCur(asgList));
        break;
      } else if (streq(stemp, "-nr")) {
        listDestroy(responsibilityList);
        autoConfigureResponsibilities();
        sprintf(stemp, "Grading_%s_%s", classId, asgId);
        responsibilityList = tableGetList(graderTable, stemp, " ");
      } else if (streq(stemp, "-cr")) {
        debugPrint("Here are the possible grading sections (that you are responsible for):"); // print out the sections and their maxpts
        for (int i = 1; i <= numSections; i++) {
          sprintf(stemp, "%d", i);
          if (!listContains(responsibilityList, stemp)) continue;
          debugPrint("%d: %s", i, tableGet(asgTable, stemp));
          sprintf(stemp, "%d.maxpts", i);
          debugPrint("\tMax Points: %s", tableGet(asgTable, stemp));
        }
        sprintf(stemp, "Grading_%s_%s", classId, asgId);
        printf("Your responsibilities are sections:\n%s\n", tableGet(graderTable, stemp)); // print the value in the key containing all sections
      } else if (!strncmp(stemp, "-gs", 3) || !strncmp(stemp, "-cg", 3)) { // check if it starts with -gs or -cg
        if (!isdigit(stemp[3])) {
          printf("This is not a valid section\n");
          continue;
        }
        int stop = -1;
        for (int i = 3; i < strlen(stemp); i++) { // check that we are trying to grade a valid section
          if (!isdigit(stemp[i])) {
            stop = i;
          }
        }
        if (stop == -1) stop = strlen(stemp);
        stemp[stop] = '\0';
        int section = atoi(stemp + 3);
        if (section == 0 || section > numSections) {
          printf("This is not a valid section\n");
          continue;
        }
        if (!listContains(responsibilityList, stemp + 3)) { // cut off the "-cs" or "-cg"
          printf("You do not have responsibility for grading section %d\n", section);
          continue;
        }
        if (!strncmp(stemp, "-gs", 3)) {
          int i = section;
          sprintf(stemp, "%s_%d", asgId, i);
          char stemp2[201];
          requireChangeDir(asgBinDir);
          FILE *exe_test = fopen(stemp, "r");
          if (exe_test) {
            fclose(exe_test);
            sprintf(stemp2, "cp %s/%s %s/%s/%s", asgBinDir, stemp, asgDir, listGetCur(asgList), stemp);
            system(stemp2);
            requireChangeDir(asgDir);
            requireChangeDir(listGetCur(asgList));
            sprintf(stemp2, "./%s", stemp);
            system(stemp2);
            sprintf(stemp2, "rm -f %s", stemp);
            system(stemp2);
          }
        }
        sprintf(stemp, "%d", section);
        debugPrint("Section %d: %s", section, tableGet(asgTable, stemp));
        debugPrint("\tCurrent Points: %s", grade[section - 1]);
        sprintf(stemp, "%d.maxpts", section);
        debugPrint("\tMax Points: %s", tableGet(asgTable, stemp));
        debugPrint("\tNotes:\n%s\n", notes[section - 1]);
        printf("How many points for section %d? (P: perfect, C: charity / 0 if not def): ", section);
        fgets(stemp, 100, stdin);
        stemp[strlen(stemp) - 1] = '\0';
        int first = true; // we call fgets above, only call it in the loop from the second time and on
        while (1) {
          if (!first) {
            fgets(stemp, 100, stdin);
            stemp[strlen(stemp) - 1] = '\0';
          } else first = false;
          if (streq(stemp, "P")) {
            sprintf(grade[section - 1], "P");
            debugPrint("Giving %s a perfect score for section %d", listGetCur(asgList), section);
            break;
          } else if (streq(stemp, "C")) {
            sprintf(grade[section - 1], "C");
            debugPrint("Giving %s charity points for section %d", listGetCur(asgList), section);
            break;
          }
          int start = -1;
          stop = -1;
          for (int i = 0; i < strlen(stemp); i++) {
            if (isdigit(stemp[i])) {
              start = i;
              break;
            }
          }
          if (start == -1) {
            printf("Please enter a valid score for section %d: ", section);
            continue;
          }
          for (int i = 0; i < strlen(stemp); i++) {
            if (!isdigit(stemp[i])) {
              stop = i;
              break;
            }
          }
          if (stop == -1) stop = strlen(stemp);
          stemp[stop] = '\0';
          int tempGrade = atoi(stemp + start);
          sprintf(stemp, "%d.maxpts", section);
          if (tempGrade >= 0 && tempGrade < tableGetInt(asgTable, stemp)) {
            sprintf(grade[section - 1], "%d", tempGrade);
            debugPrint("Giving %s %d points for section %d", listGetCur(asgList), tempGrade, section);
            break;
          } else if (tempGrade == tableGetInt(asgTable, stemp)) {
            sprintf(grade[section - 1], "P");
            debugPrint("Giving %s a perfect score for section %d", listGetCur(asgList), section);
            break;
          }
          printf("Please enter a valid score for section %d: ", section);
        }

        //Enter notes after entering grade:

        sprintf(stemp, "%d", section);
        debugPrint("Section %d: %s", section, tableGet(asgTable, stemp));
        debugPrint("\tCurrent Points: %s", grade[section - 1]);
        sprintf(stemp, "%d.maxpts", section);
        debugPrint("\tMax Points: %s", tableGet(asgTable, stemp));
        debugPrint("\tNotes:\n%s\n", notes[section - 1]);
        int numDesc = 0; // number of pre-defined descriptions for this section
        while (1) {
          numDesc++; // case for 1 is assumed to be covered since section.desc.1 is required for auto to run
          sprintf(stemp, "%d.desc.%d", section, numDesc + 1);
          if (!tableGet(asgTable, stemp)) break;
        }
        debugPrint("Custom descriptions include:");
        for (int i = 1; i <= numDesc; i++) {
          sprintf(stemp, "%d.desc.%d", section, i);
          char stemp2[4001];
          sprintf(stemp2, "%s", tableGet(asgTable, stemp));
          debugPrint("%d:", i);
          printf("\t");
          for (int j = 0; j < strlen(stemp2); j++) {
            if (stemp2[j] == '\\') {
              printf("\n");
              printf("\t");
              j++;
            } else {
              printf("%c", stemp2[j]);
            }
          }
          printf("\n");
        }
        printf("\nWould you like to enter a custom message or one of the default ones?\n");
        printf("Please type a number for the above description you would like or 'c' for custom: ");
        char predefstr[101] = ""; // emptry string is custom
        while (1) {
          fgets(stemp, 100, stdin);
          stemp[strlen(stemp) ? strlen(stemp) - 1 : 0] = '\0';
          char stemp2[101];
          sprintf(stemp2, "%d.desc.%s", section, stemp);
          if (streq(stemp, "c")) {
            break;
          } else if (tableGet(asgTable, stemp2)) {
            sprintf(predefstr, stemp2);
            break;
          } else {
            printf("Please enter a valid description selection: ");
          }
        }
        if (!predefstr[0]) {
          printf("\nPlease enter your comments followed by an empty line:\n");
          int noteSize = 0;
          notes[section - 1][0] = '\0'; // clear the current note for this section
          do {
            noteSize += strlen(fgets(stemp, 100, stdin));
            if (strlen(stemp) < 2) break;
            if (noteSize < 4001) strcat(notes[section - 1], stemp);
          } while(1);
        } else {
          char newLineParse[4001]; // convert "\\n" to "\n"
          int tempPointer1 = 0;
          int tempPointer2 = 0;
          sprintf(newLineParse, "%s", tableGet(asgTable, predefstr)); // the key for the description we want
          while (tempPointer1 < strlen(newLineParse)) {
            if (newLineParse[tempPointer1] == '\\') {
              notes[section - 1][tempPointer2++] = newLineParse[++tempPointer1] == 'n' ? '\n' : newLineParse[tempPointer1];
              tempPointer1++;
            } else {
              notes[section - 1][tempPointer2++] = newLineParse[tempPointer1++];
            }
          }
          notes[section - 1][tempPointer2] = '\0';
        }
        debugPrint("New note for section %d:\n%s\n", section, notes[section - 1]);
        // end entering notes after entering grade

      } else if (!strncmp(stemp, "-cn", 3)) {
        if (!isdigit(stemp[3])) {
          printf("This is not a valid section\n");
          continue;
        }
        int stop = -1;
        for (int i = 3; i < strlen(stemp); i++) { // check that we are trying to grade a valid section
          if (!isdigit(stemp[i])) {
            stop = i;
          }
        }
        if (stop == -1) stop = strlen(stemp);
        stemp[stop] = '\0';
        int section = atoi(stemp + 3);
        if (section == 0 || section > numSections) {
          printf("This is not a valid section\n");
          continue;
        }
        if (!listContains(responsibilityList, stemp + 3)) { // cut off the "-cg"
          printf("You do not have responsibility for grading section %d\n", section);
          continue;
        }
        sprintf(stemp, "%d", section);
        debugPrint("Section %d: %s", section, tableGet(asgTable, stemp));
        debugPrint("\tCurrent Points: %s", grade[section - 1]);
        sprintf(stemp, "%d.maxpts", section);
        debugPrint("\tMax Points: %s", tableGet(asgTable, stemp));
        debugPrint("\tNotes:\n%s\n", notes[section - 1]);
        int numDesc = 0; // number of pre-defined descriptions for this section
        while (1) {
          numDesc++; // case for 1 is assumed to be covered since section.desc.1 is required for auto to run
          sprintf(stemp, "%d.desc.%d", section, numDesc + 1);
          if (!tableGet(asgTable, stemp)) break;
        }
        debugPrint("Custom descriptions include:");
        for (int i = 1; i <= numDesc; i++) {
          sprintf(stemp, "%d.desc.%d", section, i);
          char stemp2[4001];
          sprintf(stemp2, "%s", tableGet(asgTable, stemp));
          debugPrint("%d:", i);
          printf("\t");
          for (int j = 0; j < strlen(stemp2); j++) {
            if (stemp2[j] == '\\') {
              printf("\n");
              printf("\t");
              j++;
            } else {
              printf("%c", stemp2[j]);
            }
          }
          printf("\n");
        }
        printf("\nWould you like to enter a custom message or one of the default ones?\n");
        printf("Please type a number for the above description you would like or 'c' for custom: ");
        char predefstr[101] = ""; // emptry string is custom
        while (1) {
          fgets(stemp, 100, stdin);
          stemp[strlen(stemp) ? strlen(stemp) - 1 : 0] = '\0';
          char stemp2[101];
          sprintf(stemp2, "%d.desc.%s", section, stemp);
          if (streq(stemp, "c")) {
            break;
          } else if (tableGet(asgTable, stemp2)) {
            sprintf(predefstr, stemp2);
            break;
          } else {
            printf("Please enter a valid description selection: ");
          }
        }
        if (!predefstr[0]) {
          printf("\nPlease enter your comments followed by an empty line:\n");
          int noteSize = 0;
          notes[section - 1][0] = '\0'; // clear the current note for this section
          do {
            noteSize += strlen(fgets(stemp, 100, stdin));
            if (strlen(stemp) < 2) break;
            if (noteSize < 4001) strcat(notes[section - 1], stemp);
          } while(1);
        } else {
          char newLineParse[4001]; // convert "\\n" to "\n"
          int tempPointer1 = 0;
          int tempPointer2 = 0;
          sprintf(newLineParse, "%s", tableGet(asgTable, predefstr)); // the key for the description we want
          while (tempPointer1 < strlen(newLineParse)) {
            if (newLineParse[tempPointer1] == '\\') {
              notes[section - 1][tempPointer2++] = newLineParse[++tempPointer1] == 'n' ? '\n' : newLineParse[tempPointer1];
              tempPointer1++;
            } else {
              notes[section - 1][tempPointer2++] = newLineParse[tempPointer1++];
            }
          }
          notes[section - 1][tempPointer2] = '\0';
        }
        debugPrint("New note for section %d:\n%s\n", section, notes[section - 1]);
      } else if (streq(stemp, "-cs")) {
        debugPrint("Your grades and comments are:\n");
        for (int i = 1; i <= numSections; i++) {
          sprintf(stemp, "%d", i);
          if (!listContains(responsibilityList, stemp)) continue;
          char stemp2[101];
          sprintf(stemp2, "%s.maxpts", stemp);
          debugPrint("Section %d: Grade for %s is %s / %s and notes are:\n%s\n",
              i, tableGet(asgTable, stemp), grade[i - 1], tableGet(asgTable, stemp2), notes[i - 1]);
        }
      } else if (streq(stemp, "-rs")) {
        for (int i = 1; i <= numSections; i++) {
          sprintf(stemp, "%d", i);
          if (!listContains(responsibilityList, stemp)) continue; // if not responsibility, don't bother
          sprintf(stemp, "grade.%d", i);
          if (strcmp(tableGet(studentTable, stemp), "U") != 0) continue; // if graded, don't bother
          sprintf(stemp, "%s_%d", asgId, i);
          char stemp2[201];
          requireChangeDir(asgBinDir);
          FILE *exe_test = fopen(stemp, "r");
          if (exe_test) {
            fclose(exe_test);
            sprintf(stemp2, "cp %s/%s %s/%s/%s", asgBinDir, stemp, asgDir, listGetCur(asgList), stemp);
            system(stemp2);
            requireChangeDir(asgDir);
            requireChangeDir(listGetCur(asgList));
            sprintf(stemp2, "./%s", stemp);
            system(stemp2);
            sprintf(stemp2, "rm -f %s", stemp);
            system(stemp2);
          }
        }
        requireChangeDir(asgDir);
        requireChangeDir(listGetCur(asgList));
      } else {
        debugPrint("system(%s)", stemp);
        system(stemp);
        //printf("Invalid command\n");
      }
    }
    listDestroy(responsibilityList);
    printf("\nFinished grading %s\n", listGetCur(asgList));
    if (++count == 4) {
      printf("Would you like to quit autograde? [y/<anything>]: ");
      char stopcont[101] = {};
      fgets(stopcont, 100, stdin);
      stopcont[strlen(stopcont) - 1] = '\0';
      if (streq(stopcont, "y")) {
        break;
      }
      count = 0;
    }
  } while(autocont);
  if (wasSafeToGrade) studentWrite(1);
}

void autoCompile() {
  if (!listMoveFront(asgList)) return;
  requireChangeDir(asgBinDir);
  char tempstr[1024];
  sprintf(tempstr, "%s.%s.csv", classId, asgId);
  FILE *fullGradeSheet = fopen(tempstr, "w");
  fprintf(fullGradeSheet, "SIS Login ID,%s\n", asgId);
  int maxscore = 0;
  for (int i = 0; i < numSections; i++) {
    sprintf(tempstr, "%d.maxpts", i + 1);
    maxscore += tableGetInt(asgTable, tempstr);
  }
  //TODO: do we need max points possible as a row?
  sprintf(tempstr, "%s.%s.grade.txt", classId, asgId);
  FILE *fullGradeList = fopen(tempstr, "w");
  fprintf(fullGradeList, "%s\n", tempstr);
  char compile_status[100000] = {}; // string informing whether grading complete
  int not_compiled = 0; // number of grade files not compiled
  do {
    studentRead();
    system("rm -f grade.txt");

    char *cheatedstr = tableGet(studentTable, "cheated");
    if (cheatedstr && cheatedstr[0] == 'Y') { // give them the cheated file
      fprintf(fullGradeSheet, "%s@ucsc.edu,%d\n", studentId, 0); // add to the spreadsheet
      system("cp ../../bin/moss/canned_grade.txt temp_grade.txt");
      FILE *canned_grade = fopen("grade.txt", "w");
      FILE *temp_grade = fopen("temp_grade.txt", "r");
      fprintf(canned_grade, "To %s:\n", studentId);
      while (fgets(tempstr, 1023, temp_grade)) fprintf(canned_grade, tempstr);
      fclose(temp_grade);
      fclose(canned_grade);
      system("rm temp_grade.txt");
      continue; // they now have a "grade.txt" file
    }

    int ungraded = 0; // 1 if there is an ungraded section
    for (int i = 1; i <= numSections; i++) { // check that there are no Ungraded assignments
      char tempstr2[128];
      sprintf(tempstr2, "grade.%d", i);
      char *tableret = tableGet(studentTable, tempstr2);
      sprintf(tempstr, tableret);
      if (tableret == NULL || streq(tempstr, "U")) {
        ungraded = 1;
      }
    }
    if (ungraded) {
      not_compiled++;
      strcat(compile_status, tableGet(studentTable, "user.name"));
      strcat(compile_status, " not compiled\n");
      continue;
    }
    FILE *gradeFile = fopen("grade.txt", "w");
    fprintf(gradeFile, "%s\n", currentPath());
    fprintf(fullGradeList, "%s\n", currentPath());
    fprintf(gradeFile, "CLASS:\t%s\nASG:\t%s\nGRADERS:", classId, asgId);
    fprintf(fullGradeList, "CLASS:\t%s\nASG:\t%s\nGRADERS:", classId, asgId);
    fprintf(gradeFile, "\tIsaak Joseph Cherdak <icherdak>\n"); // TEMPORARY HARDCODE until more configuration exists
    fprintf(gradeFile, "\tMia Gabrielle Altieri <mgaltier>\n"); // TEMPORARY HARDCODE until more configuration exists
    fprintf(gradeFile, "\tAlexander Zuo-Tao Lue <alue>\n"); // TEMPORARY HARDCODE until more configuration exists
    fprintf(fullGradeList, "\tIsaak Joseph Cherdak <icherdak>\n"); // TEMPORARY HARDCODE until more configuration exists
    fprintf(fullGradeList, "\tMia Gabrielle Altieri<mgaltier>\n"); // TEMPORARY HARDCODE until more configuration exists
    fprintf(fullGradeList, "\tAlexander Zuo-Tao Lue <alue>\n"); // TEMPORARY HARDCODE until more configuration exists
    fprintf(gradeFile, "STUDENT:\t%s <%s>\n", tableGet(studentTable, "user.name"), studentId);
    fprintf(fullGradeList, "STUDENT:\t%s\n", studentId);
    if (0) {
      fclose(gradeFile);
      continue;
    }
    int score = 0;
    for (int i = 1; i <= numSections; i++) { // add up score breakdown
      char tempstr2[128];
      sprintf(tempstr2, "grade.%d", i);
      sprintf(tempstr, tableGet(studentTable, tempstr2));
      if (streq(tempstr, "C")) {
        sprintf(tempstr, "%d.charity", i);
        if (tableGet(asgTable, tempstr)) score += tableGetInt(asgTable, tempstr); // if charity doesn't exist, give 0 pts
      } else if (streq(tempstr, "P")) {
        sprintf(tempstr, "%d.maxpts", i);
        //printf("%d.maxpts = %d\n", i, tableGetInt(asgTable, tempstr));
        score += tableGetInt(asgTable, tempstr); // maxpts for this section
      } else {
        score += atoi(tempstr); // we assume the grade given is valid and don't check bounds (in later versions we will)
      }
    }
    fprintf(fullGradeSheet, "%s@ucsc.edu,%d\n", studentId, score); // add to the spreadsheet
    fprintf(gradeFile, "SCORE:\t%d / %d (%d%%)\n", score, maxscore, maxscore ? 100 * score / maxscore : 0);
    fprintf(fullGradeList, "SCORE:\t%d / %d (%d%%)\n", score, maxscore, maxscore ? 100 * score / maxscore : 0);

    fprintf(gradeFile, "\nGRADE BREAKDOWN:\n");
    fprintf(fullGradeList, "\nGRADE BREAKDOWN:\n");

    for (int i = 1; i <= numSections; i++) {
      char tempstr2[128];
      sprintf(tempstr2, "grade.%d", i);
      sprintf(tempstr, tableGet(studentTable, tempstr2));
      if (streq(tempstr, "C")) {
        sprintf(tempstr, "%d.charity", i);
        if (tableGet(asgTable, tempstr)) {
          fprintf(gradeFile, "%d / ", tableGetInt(asgTable, tempstr)); // if charity doesn't exist, give 0 pts
          fprintf(fullGradeList, "%d / ", tableGetInt(asgTable, tempstr)); // if charity doesn't exist, give 0 pts
        } else {
          fprintf(gradeFile, "%d / ", 0);
          fprintf(fullGradeList, "%d / ", 0);
        }
      }else if (streq(tempstr, "P")) {
        sprintf(tempstr, "%d.maxpts", i);
        //printf("%d.maxpts = %d\n", i, tableGetInt(asgTable, tempstr));
        fprintf(gradeFile, "%s / ", tableGet(asgTable, tempstr)); // maxpts for this section
        fprintf(fullGradeList, "%s / ", tableGet(asgTable, tempstr)); // maxpts for this section
      } else {
        fprintf(gradeFile, "%s / ", tempstr); // we assume the grade given is valid and don't check bounds (in later versions we will)
        fprintf(fullGradeList, "%s / ", tempstr); // we assume the grade given is valid and don't check bounds (in later versions we will)
      }
      sprintf(tempstr, "%d.maxpts", i);
      fprintf(gradeFile, "%s | ", tableGet(asgTable, tempstr));
      fprintf(fullGradeList, "%s | ", tableGet(asgTable, tempstr));

      sprintf(tempstr, "%d", i);
      fprintf(gradeFile, "%s\n", tableGet(asgTable, tempstr));
      fprintf(fullGradeList, "%s\n", tableGet(asgTable, tempstr));
    }
    fprintf(gradeFile, "\n");
    fprintf(fullGradeList, "\n");
    for (int i = 1; i <= numSections; i++) {
      sprintf(tempstr, "%d", i);
      fprintf(gradeFile, "\nNotes for %s:\n====================\n", tableGet(asgTable, tempstr));
      fprintf(fullGradeList, "\nNotes for %s:\n====================\n", tableGet(asgTable, tempstr));
      sprintf(tempstr, "notes.%d", i);
      char tempstr2[400];
      int tempstrp = 0; // pointer to current index
      sprintf(tempstr2, "%s", tableGet(studentTable, tempstr) );
      while (tempstrp < strlen(tempstr2)) {
        if (tempstr2[tempstrp] == '\\') { // because we have to expand the '\n' character to "\\n"
          fputc('\n', gradeFile);
          fputc('\n', fullGradeList);
          tempstrp += 2;
        } else {
          fputc(tempstr2[tempstrp], gradeFile);
          fputc(tempstr2[tempstrp], fullGradeList);
          tempstrp++;
        }
      }
      fprintf(gradeFile, "\n====================\n");
      fprintf(fullGradeList, "\n====================\n");
    }
    fprintf(gradeFile,  "\nPlease check piazza for details on grading and a rubric\n"); // hardcoded stuff, use config eventually
    //fprintf(fullGradeList,  "\nPiazza post: https://piazza.com/class/ixpl5nsw9fnta?cid=524\n"); // hardcoded stuff, use config eventually

    fclose(gradeFile);
    fprintf(fullGradeList, "==================================================\n");
  } while (listMoveNext(asgList));
  listMoveFront(asgList);
  fclose(fullGradeSheet);
  fclose(fullGradeList);
  if (!strlen(compile_status)) strcat(compile_status, "Finished grading all students\n");
  else {
    strcat(compile_status, "Grading unfinished\n");
    char uncompiled_str[16];
    sprintf(uncompiled_str, "%d", not_compiled);
    strcat(compile_status, uncompiled_str);
    strcat(compile_status, " total remaining\n");
  }
  debugPrint("Compile Status:\n\n%s\n", compile_status);

}

/////////////////////////////////////////////////////////////////////

//deprecated
void testGrade(char *dir) {
  if (strncmp(dir, "pa2", 4)) {
    printf("This method is not yet ready\n");
    return;
  }
  char path[500];
  FILE *fp;
  FILE *fp2;
  struct dirent **fileList;
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s", dir);
  int filecount;
  if (chdir(path) != 0) {
    printf("Trouble switching to the specified directory\n");
    return;
  }
  filecount = scandir(".", &fileList, NULL, alphasort);
  char *fl[filecount];
  for (int l = 0; l < filecount; l++) {
    fl[l] = fileList[l]->d_name;
  }
  char temps[501];
  char temps1[501];
  char mode = '2';
  List *l = listCreate(fileList, 2);
  List *ltemp = NULL;
  if (!l) {
    printf("Error compiling list");
    return;
  }
  sprintf(temps, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s/%s", dir, l->first->sdir);
  chdir(temps);
  printf("Type '-h' for help\n");
  Node *temp = l->first;
  while (1) {
    printf("auto> ");
    gets(temps1);
    if (strncmp(temps1, "-h", 2) == 0) {
      printf("List of commands:\n-f - go to first directory\n-l - go to last directory\n-n - go to next directory\n-p - go to previous directory\n-ce - check Extrema.java\n-ct - check tests.txt\n-cd - check diff*\n-co - check out*\n-cg - check peformance.txt and design.txt\n-cm - check Makefile\n-c - check major files\n-m - change modes\n-ftr - filter the list to only have directories that contain (or don't by adding '!') a file\n-ft - fast filter that accepts direct argument and otherwise works like -ftr\n-vdt - open design.txt in vim\n-vpt - open performance.txt in vim\n-vbt - open bugs.txt\n-vgt - open grade.txt in vim\n-e - exit the program securely\n-h - bring up help\ntype anything else and it will be run as a unix command\n");
    } else if (strncmp(temps1, "-f", 3) == 0) {
      temp = l->first;
      chdir("..");
      chdir(temp->sdir);
    } else if (strncmp(temps1, "-n", 3) == 0) {
      if (temp->next) {
        temp = temp->next;
        chdir("..");
        chdir(temp->sdir);
      } else printf("No next directory\n");
    } else if (strncmp(temps1, "-p", 3) == 0) {
      if (temp->prev) {
        temp = temp->prev;
        chdir("..");
        chdir(temp->sdir);
      } else printf("No previous directory\n");
    } else if (strncmp(temps1, "-l", 3) == 0) {
      temp = l->last;
      chdir("..");
      chdir(temp->sdir);
    } else if (strncmp(temps1, "-m", 3) == 0) {
      do {
        printf("Enter a mode: 0 for ungraded, 1 for graded, 2 for mixed: ");
        mode = getchar();
      } while(mode < 48 || mode > 50);
      ltemp = listCreate(fileList, (int) mode - 48);
      if (!ltemp) {
        printf("%s mode has has no directories, try %s mode or mixed mode\n", mode == 48 ? "ungraded" : "graded", mode == 49 ? "ungraded" : "graded");
      } else {
        listDestroy(l);
        l = ltemp;
        ltemp = NULL;
        temp = l->first;
        chdir("..");
        chdir(temp->sdir);
        printf("Changing to mode %d\n", (int) mode - 48);
      }
    } else if (strncmp(temps1, "-e", 3) == 0) {
      getGrades(dir);
      printf("Exiting program\n");
      break;
    } else if (strncmp(temps1, "-vpt", 5) == 0) {
      fp = fopen("performance.txt", "r");
      if (!fp) {
        sprintf(temps1, "cp /afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/performance.txt .", dir);
        system(temps1);
      } else fclose(fp);
      system("vi performance.txt");
    } else if (strncmp(temps1, "-vdt", 5) == 0) {
      fp = fopen("design.txt", "r");
      if (!fp) {
        sprintf(temps1, "cp /afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/design.txt .", dir);
        system(temps1);
        fp = fopen("design.txt", "a");
        fprintf(fp, "\n\n");
        fp2 = fopen("design.temp", "r");
        if (fp2) while (fgets(temps1, 500, fp2)) {
          fprintf(fp, "%s", temps1);
        }
        if (fp2) fclose(fp2);
      }
      fclose(fp);
      system("vi design.txt");
    } else if (strncmp(temps1, "-vgt", 5) == 0) {
      system("vi grade.txt");
      //} else if (strncmp(temps1, "-pvgt", 6) == 0) {
      //  fp = fopen("grade.txt", "w");
      //  fprintf(fp, "%s points\n", (strncmp(dir, "pa3", 4) == 0 || strncmp(dir, "pa2", 4) == 0 || strncmp(dir, "pa1", 4)) ? "80/80" : "100/100");
      //  fclose(fp);i
  } else if (strncmp(temps1, "-vbt", 5) == 0) {
    system("vi bugs.txt");
  } else if (strncmp(temps1, "-c", 3) == 0) {
    system("more tests.txt");
    system("more *");
    fp = fopen("bugs.txt", "r");
    printf("%s", fp ? "::::::::::::::\nBUGS.TXT EXISTS!\n" : "");
    if (fp) fclose(fp);
  } else if (strncmp(temps1, "-cc", 4) == 0) {
    system("more tests.txt bugs.txt performance.txt design.txt design.temp diff1 diff21 diff31 diff41 Search.java Makefile README tests.txt");
  } else if (strncmp(temps1, "-ct", 4) == 0) {
    system("more tests.txt");
  } else if (strncmp(temps1, "-cm", 4) == 0) {
    system("more Makefile");
  } else if (strncmp(temps1, "-cd", 4) == 0) {
    system("more diff*");
  } else if (strncmp(temps1, "-cg", 4) == 0) {
    system("more performance.txt design.txt");
  } else if (strncmp(temps1, "-ce", 4) == 0) {
    system("more Search.java");
  } else if (strncmp(temps1, "-co", 4) == 0) {
    system("more out*");
  } else if (strncmp(temps1, "-pos", 5) == 0) {
    printf("Your current position is %d out of %d\n", listGetPos(l), listGetSize(l));
  } else if (strncmp(temps1, "-ftr", 5) == 0) {
    printf("Please enter the name of the text file you would like to filter for (add '!' to front of name to make it filter to directories without the file): ");
    fgets(temps1, 500, stdin);
    //printf("The thing being filtered is: %s which is size %d\n", temps1, strlen(temps1));
    int tempint = listGetSize(l);
    listFilter(l, dir, temps1);
    if (tempint != listGetSize(l)) {
      temp = l->first;
      chdir("..");
      chdir(temp->sdir);
    }
  } else if (strncmp(temps1, "-ft ", 4) == 0) {
    //printf("Please enter the name of the text file you would like to filter for (add '!' to front of name to make it filter to directories without the file): ");
    strncpy(temps, temps1 + 4, 500);
    temps[strlen(temps)] = '\n';
    temps[strlen(temps) + 1] = '\0';
    //printf("The thing being filtered is: %s which is size %d\n", temps, strlen(temps));
    int tempint = listGetSize(l);
    listFilter(l, dir, temps);
    if (tempint != listGetSize(l)) {
      temp = l->first;
      chdir("..");
      chdir(temp->sdir);
    }
  } else {
    //printList(l);
    //printf("The size of the list is %d\n", listGetSize(l));
    system(temps1);
  }
  }
  for (int i = 0; i < filecount; i++)
    free(fileList[i]);
  free(fileList);
  listDestroy(l);
}

void outdatedAutoGrade(char *dir) {
  //printf("This method is not yet ready\n");
  //return;
  if (1 || strncmp(dir, "pa2", 4)) {
    printf("Auto grade function is not yet available for this directory\n");
    return;
  }
  char execfile[10];
  //sprintf(dir, "pa4");
  //printf("%s\n", dir);
  sprintf(execfile, "m%s", dir);
  //printf("%s\n", dir);
  char path[500];
  FILE *fp;
  FILE *fp2;
  FILE *fp3;
  FILE *fp4 = NULL;
  struct dirent **fileList;
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s", dir);
  //printf("%s\n", path);
  int filecount;
  chdir("..");
  if (chdir(dir) != 0) {
    printf("Trouble switching to %s\n", dir);
    return;
  }
  filecount = scandir(".", &fileList, NULL, alphasort);
  char temp[501] = "";
  char temp1[501] = "";
  int diff = 0;
  for (int i = 2; i < filecount; i++) { //start at i = 2 because we are ignoring the "." and ".." directories
    //printf("test\n");
    diff = 0;
    if (chdir(fileList[i]->d_name) != 0) {
      printf("Error changing directories\n");
      return;
    }
    sprintf(temp, "cp /afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/%s /afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s/%s", dir, execfile, dir, fileList[i]->d_name);
    system(temp);
    fp = fopen("tests.txt", "w");
    fp4 = fopen("design.temp", "w");
    system("make");
    fp3 = fopen("Search", "r");
    fprintf(fp, "Makefile Test %s\n", fp3 ? "Passed" : "Failed");
    if (fp3) {
      fclose(fp3);
    } else {
      fprintf(fp4, "-5 points - Makefile doesn't make.");
      sprintf(temp, "cp /afs/cats.ucsc.edu/users/d/icherdak/cs12b_pt.s15/%s/mfile .", dir);
      system(temp);
      system("mfile");
      system("rm mfile");
    }
    sprintf(temp, "timeout 25 %s", execfile);
    system(temp);
    fp2 = fopen("diff1", "r");
    fprintf(fp, "User Input Tests %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
    if (fp2) fclose(fp2);
    int scount = 0; // this is how we know that both untit tests passed special tests
    fp2 = fopen("diff21", "r");
    fp3 = fopen("diff22", "r");
    fprintf(fp, "Unit Tests 1 %s\n", (fp2 && fgetc(fp2) == EOF || fp3 && fgetc(fp3) == EOF) ? (fp2 && fgetc(fp2) == EOF ? "Normal Tests Passed" : "Special Tests Passed") : "Failed Tests");
    if (fp3 && feof(fp3)) scount = 1;
    if (fp2) fclose(fp2);
    if (fp3) fclose(fp3);
    fp2 = fopen("diff31", "r");
    fp3 = fopen("diff32", "r");
    fprintf(fp, "Unit Tests 2 %s\n", (fp2 && fgetc(fp2) == EOF || fp3 && fgetc(fp3) == EOF) ? (fp2 && fgetc(fp2) == EOF ? "Normal Tests Passed" : "Special Tests Passed") : "Failed Tests");
    if (fp3 && feof(fp3)) scount = 1;
    if (fp2) fclose(fp2);
    if (fp3) fclose(fp3);
    fp2 = fopen("diff41", "r");
    fp3 = fopen("diff42", "r");
    fprintf(fp, "Multi Target %s\n", (fp2 && fgetc(fp2) == EOF || fp3 && fgetc(fp3) == EOF) ? (fp2 && fgetc(fp2) == EOF ? "Normal Tests Passed" : "Special Tests Passed") : "Tests Failed");
    if (fp3 && feof(fp3)) scount = 1;
    if (scount) {
      //  if (!fp4) fp4 = fopen("design.temp", "w");
      fprintf(fp4, "-10 points - Program doesn't support line numbers\n\n");
    }
    if (!(fp2 && feof(fp2) || fp3 && feof(fp3))) {
      //  if (!fp4) fp4 = fopen("design.temp", "w");
      //fprintf(fp4, "-5 points - Program doesn't support variable arguments\n\n");
    }
    if (fp2) fclose(fp2);
    if (fp3) fclose(fp3);
    if (fp) fclose(fp);
    fp = fopen("README", "r");
    fp2 = fopen("Makefile", "r");
    fp3 = fopen("Search.java", "r");
    if (!fp || !fp2 || !fp3) {
      //  if (!fp4) fp4 = fopen("design.temp", "w");
      if (!fp) fprintf(fp4, "-3 points - Missing/incorrectly named file: README\n\n");
      if (!fp2) fprintf(fp4, "-3 points - Missing/incorrectly named file: Makefile\n\n");
      if (!fp3) fprintf(fp4, "-3 points - Missing/incorrectly named file: Search.java\n\n");
    }
    if (fp) fclose(fp);
    if (fp2) fclose(fp2);
    if (fp3) fclose(fp3);
    if (fp4) fclose(fp4);
    /*fp2 = fopen("diff3", "r");
      fprintf(fp, "Advanced Unit Tests %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
      if (fp2 && fgetc(fp2) == EOF) diff++;
      if (fp2) fclose(fp2);
      fp2 = fopen("diff4", "r");
      fprintf(fp, "Credibility Test %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
      if (fp2 && fgetc(fp2) == EOF) diff++;
      if (fp2) fclose(fp2);*/

    /*
       fp2 = fopen("diff5", "r");
       fprintf(fp, "Test 5 %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
       if (fp2 && fgetc(fp2) == EOF) diff++;
       if (fp2) fclose(fp2);
       fp2 = fopen("diff6", "r");
       fprintf(fp, "\"make\" Test %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
       if (fp2 && fgetc(fp2) == EOF) diff++;
       if (fp2) fclose(fp2);
       fp2 = fopen("diff7", "r");
       fprintf(fp, "\"make submit\" Test %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
       if (fp2 && fgetc(fp2) == EOF) diff++;
       if (fp2) fclose(fp2);
       fp2 = fopen("diff8", "r");
       fprintf(fp, "\"make clean\" Test %s\n", (!fp2 || fp2 && fgetc(fp2) != EOF) ? "Failed" : "Passed");
       if (fp2 && fgetc(fp2) == EOF) diff++;
       if (fp2) fclose(fp2);
       */

    /*if (0 && diff == 8) {
      fp = fopen("grade.txt", "r");
      if (!fp) {
      fp = fopen("grade.txt", "w");
      fprintf(fp, "\n\nAll Test Passed");
      }
      fclose(fp);
      }*/
    sprintf(temp, "rm %s", execfile);
    system(temp);
    system("rm -f *.class Search");
    printf("Finished grading %s for %s\n", fileList[i]->d_name, dir);
    chdir("..");
  }
  for (int i = 0; i < filecount; i++)
    free(fileList[i]);
  free(fileList);
  printf("Grading routine finished\n");
}

void sendMail() {

  changeDir(asgBinDir);
  FILE *mscript = fopen("mailscript", "w");
  fprintf(mscript, "#!/bin/bash\ncd %s\n", asgDir);
  int count = 0; // how many students will be emailed
  listMoveFront(asgList);
  while(1) {
    changeDir(asgDir);
    requireChangeDir(listGetCur(asgList));
    FILE *fp = fopen("grade.txt", "r");
    if (fp) {
      fclose(fp);
      fprintf(mscript, "cd %s\necho \"Mailing %s@ucsc.edu\"\nmailx -s \"%s grade for %s\" %s@ucsc.edu < grade.txt\ncd ..\nsleep 2\n",
          listGetCur(asgList), listGetCur(asgList), listGetCur(asgList), asgId, listGetCur(asgList));
      count++;
    } else {
      autoWarn("%s: No grade.txt", listGetCur(asgList));
    }
    if (!listMoveNext(asgList)) break;
  }
  fprintf(mscript, "echo \"Finished mailing all students\"");
  fclose(mscript);
  listMoveFront(asgList);
  changeDir(asgBinDir);
  while(1) {
    autoPrint("Approximate runtime: %d minute%s %d second%s, Would you like to email all students that have been graded? <y/n> [y]",
        (count * 2) / 60, ((count * 2) / 60) == 1 ? "" : "s", (count * 2) % 60, ((count * 2) % 60) == 1 ? "" : "s");
    char ask = getchar();
    //debugPrint("ask is %c with %d", ask, ask);
    if (ask == 'y') {
      system("chmod 700 mailscript");
      system("./mailscript");
      system("rm mailscript");
      debugPrint("Mail Routine Complete");
      break;
    } else if (ask =='n') {
      system("rm mailscript");
      debugPrint("Exiting Program");
      break;
    } else if (ask == '\n') {
      system("chmod 700 mailscript");
      system("./mailscript");
      system("rm mailscript");
      debugPrint("Mail Routine Complete");
      break;
    } else {
      printf("Invalid Command!\nPlease Enter An Appropriate Character <y/n> [y]\n");
    }
  }

  return; // the rest is old stuff

  /*

  //  printf("Mail method not yet available\n");
  //  return;
  char path[500];
  //  sprintf(path, "/afs/cats.ucsc.edu/class/cmps011-pt.w15/bin/%s/mail_all", dir);
  //  FILE *fp = fopen(path, "w");
  FILE *fp;
  struct dirent **fileList;
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s", dir);
  int filecount;
  chdir("..");
  if (chdir(dir) != 0) {
  printf("Trouble switching to the specified directory\n");
  return;
  }
  filecount = scandir(".", &fileList, NULL, alphasort);
  FILE *fp2 = fopen("/afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/mailscript", "w");
  fprintf(fp2, "#!/bin/csh\ncd /afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s\n", dir);
  int mailcount = 0;
  //  char temp[501];
  //  char commands[filecount - 2][501];
  for (int i = 2; i < filecount; i++) { //start at i = 2 because we are ignoring the "." and ".." directories
  chdir(fileList[i]->d_name);
  fp = fopen("grade.txt", "r");
  if (!fp) printf("grade.txt does not exist for %s\n", fileList[i]->d_name);
  else {
  fprintf (fp2, "cd %s\necho \"Mailing %s@ucsc.edu\"\nmailx -s \"grade for %s\" %s@ucsc.edu < grade.txt\ncd ..\nsleep 3\n", fileList[i]->d_name, fileList[i]->d_name, dir, fileList[i]->d_name);
  //      strncpy(commands[i - 2], temp, 500);
  //      printf("%s\n", commands[i - 2]);
  fclose(fp);
  mailcount++;
  }
  chdir("..");
  }
  //  fclose(fp);
  for (int i = 0; i < filecount; i++)
  free(fileList[i]);
  free(fileList);
  fclose(fp2);
  chdir("/afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin");
  while(1) {
  printf("Approximate runtime: %d minute%s %d second%s, Would you like to email all students that have been graded? [y/n]\n", (mailcount * 3) / 60, ((mailcount * 3) / 60) == 1 ? "" : "s", (mailcount * 3) % 60, ((mailcount * 3) % 60) == 1 ? "" : "s");
  char ask = getchar();
  if (ask == 'y') {
  system("chmod 700 mailscript");
  system("mailscript");
  system("rm mailscript");
  printf("Mail Routine Complete\n");
  return;
  } else if (ask =='n') {
  system("rm mailscript");
  printf("Exiting Program\n");
  return;
  } else {
  printf("Invalid Command!\nPlease Enter An Appropriate Character [y/n]\n");
  }
  }

*/
}

void getGrades(char *dir) {
  if (strncmp(dir, "pa2", 4)) {
    printf("This method is currently unavailable\n");
    return;
  }
  char path[500];
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/gradelist.txt", dir);
  FILE *fp = fopen(path, "w");
  FILE *fp2;
  FILE *fp10;
  FILE *fp20;
  FILE *fp30;
  struct dirent **fileList;
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s", dir);
  int filecount;
  if (chdir(path) != 0) {
    printf("Trouble switching to the specified directory\n");
    return;
  }
  filecount = scandir(".", &fileList, NULL, alphasort);
  char temp[501];
  char temp1[501];
  int grade1, grade2, grade3, grade4;
  for (int i = 2; i < filecount; i++) { //start at i = 2 because we are ignoring the "." and ".." directories
    chdir(fileList[i]->d_name);
    fp2 = fopen("grade.txt", "r");
    //system("rm -f grade.txt");
    if (1 || !fp2) {
      if (fp2) fclose(fp2);
      fp10 = fopen("performance.txt", "r");
      fp20 = fopen("design.txt", "r");
      fp30 = fopen("bugs.txt", "r");
      if (fp10) {
        sprintf(temp1, "cp performance.txt /afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/performance/%s_performance.txt", dir, fileList[i]->d_name);
        system(temp1);
      }
      if (fp20) {
        sprintf(temp1, "cp design.txt /afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/design/%s_design.txt", dir, fileList[i]->d_name);
        system(temp1);
      }
      if (fp30) {
        sprintf(temp1, "cp bugs.txt /afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/bugs/%s_bugs.txt", dir, fileList[i]->d_name);
        system(temp1);
        fclose(fp30);
      }
      if (fp10 && fp20) {
        fp2 = fopen("grade.txt", "w");
        fscanf(fp10, "%d", &grade3);
        fscanf(fp20, "%d", &grade4);
        grade1 = grade3;
        grade2 = grade4;
        while(fgets(temp1, 500, fp10)) {
          if (temp1[0] == '-') {
            int index = 1;
            while (isdigit(temp1[index])) {
              temp[index - 1] = temp1[index];
              index++;
            }
            temp[index - 1] = '\0';
            index = strlen(temp) - 1;
            while (index >= 0) {
              //printf("Subtracting %d from %d in %s as a result of array: {%s}\n", pow(10, ((int) strlen(temp) - index - 1)) * ((int) temp[index] - 48), grade1, fileList[i]->d_name, temp);
              grade1 -= pow (10, strlen(temp) - index - 1) * ((int) temp[index] - 48);
              index--;
            }
          }
        }
        while(fgets(temp1, 500, fp20)) {
          if (temp1[0] == '-') {
            int index = 1;
            while (isdigit(temp1[index])) {
              temp[index - 1] = temp1[index];
              index++;
            }
            temp[index - 1] = '\0';
            index = strlen(temp) - 1;
            while (index >= 0) {
              //printf("Subtracting %d from %d in %s as a result of array: {%s}\n", pow(10, ((int) strlen(temp) - index - 1)) * ((int) temp[index] - 48), grade2, fileList[i]->d_name, temp);
              grade2 -= pow(10, strlen(temp) - index - 1) * ((int) temp[index] - 48);
              index--;
            }
          }
        }
        fclose(fp10);
        fp10 = fopen("performance.txt", "r");
        fgets(temp1, 500, fp10);
        fclose(fp20);
        fp20 = fopen("design.txt", "r");
        fgets(temp1, 500, fp20);
        //fscanf(fp10, "%d", &grade3);
        //fscanf(fp20, "%d", &grade4);
        fprintf(fp2, "%d/%d points\n", grade1 + grade2, grade3 + grade4);
        //printf("test1");
        fprintf(fp2, "====================\nPerformance\n====================\n\n%d/%d points\n", grade1, grade3);
        while(fgets(temp, 500, fp10)) {
          fprintf(fp2, "%s", temp);
          //printf("test2");
        }
        fprintf(fp2, "\n====================\nDesign\n====================\n\n%d/%d points\n", grade2, grade4);
        while(fgets(temp, 500, fp20)) {
          fprintf(fp2, "%s", temp);
          //printf("test3");
        }
        fprintf(fp2, "\n====================");
        if (fp2) fclose(fp2);
        fp2 = fopen("grade.txt", "r");
      } else {
        if (fp10) fclose(fp10);
        if (fp20) fclose(fp20);
      }
    }
    if (!fp2) printf("grade.txt does not exist for %s\n", fileList[i]->d_name);
    else {
      printf("Finished Compiling %s\n", fileList[i]->d_name);
      if (fgets(temp, 500, fp2)) {
        strncpy (temp1, temp, 501);
        temp1[strlen(temp1) - 1] = '\0';
        sprintf (temp, "%s: %s (Performance: %d points, Design: %d points)\n", fileList[i]->d_name, temp1, grade1, grade2);
        fputs(temp, fp);
      }
      fclose(fp2);
      fclose(fp10);
      fclose(fp20);
    }
    chdir("..");
  }
  fclose(fp);
  for (int i = 0; i < filecount; i++)
    free(fileList[i]);
  free(fileList);
}

void restoreGrades(char *dir) { //buggy
  if (strncmp(dir, "pa", 2)) {
    printf("This method is unavailable for this directory");
    return;
  }
  char path[501];
  //  sprintf(path, "/afs/cats.ucsc.edu/class/cmps011-pt.w15/bin/%s/mail_all", dir);
  //  FILE *fp = fopen(path, "w");
  //FILE *fp;
  struct dirent **fileList;
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/performance", dir);
  int filecount;
  if (chdir(path) != 0) {
    printf("Trouble switching to performance backup directory\n");
    return;
  }
  filecount = scandir(".", &fileList, NULL, alphasort);
  char temp[501];
  //char temp1[501];
  int j;
  for (int i = 2; i < filecount; i++) { //start at i = 2 because we are ignoring the "." and ".." directories
    for (j = 0; fileList[i]->d_name[j] != '_'; j++) {
      path[j] = fileList[i]->d_name[j];
    }
    path[j] = '\0';
    sprintf(temp, "cp %s_performance.txt /afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s/%s/performance.txt", path, dir, path);
    system(temp);
    printf("Restoring performance.txt for %s\n", path);
  }
  for (int i = 0; i < filecount; i++)
    free(fileList[i]);
  free(fileList);
  printf("Restored %d performance.txt files\n", filecount - 2);
  sprintf(path, "/afs/cats.ucsc.edu/class/cmps012b-pt.s15/bin/%s/design", dir);
  if (chdir(path) != 0) {
    printf("Trouble switching to design backup directory\n");
    return;
  }
  filecount = scandir(".", &fileList, NULL, alphasort);
  for (int i = 2; i < filecount; i++) { //start at i = 2 because we are ignoring the "." and ".." directories
    for (j = 0; fileList[i]->d_name[j] != '_'; j++) {
      path[j] = fileList[i]->d_name[j];
    }
    path[j] = '\0';
    sprintf(temp, "cp %s_design.txt /afs/cats.ucsc.edu/class/cmps012b-pt.s15/%s/%s/design.txt", path, dir, path);
    system(temp);
    printf("Restoring design.txt for %s\n", path);
  }
  for (int i = 0; i < filecount; i++)
    free(fileList[i]);
  free(fileList);
  printf("Restored %d design.txt files\n", filecount - 2);
  getGrades(dir);
}
