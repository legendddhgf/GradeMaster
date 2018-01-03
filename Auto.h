/*
 * Auto.h
 */

#ifndef _AUTO_H
#define _AUTO_H

#include <time.h>

#include "List.h"
#include "Table.h"
#include "Extra.h"

void autoGrade();

void sendMail();

void getGrades(char *dir);

void restoreGrades(char *dir); //buggy

void testGrade(char *dir);

bool changeDir(char *dir);

void assertChangeDir(char *dir);

void requireChangeDir(char *dir);

char* currentPath();

char* currentDir();

void autoPrompt();

void autoInput(char* result, char* prompt);

bool autoAsk(char* std);

void autoShell();

void studentWrite();

void studentRead();

void autoWrite();

void autoCompile();

#endif //_AUTO_H
