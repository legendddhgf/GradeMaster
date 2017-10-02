#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "ExtraUtil.h"

#include"AssignmentInfo.h"

#ifdef OVERRIDE_PRINTF
#define printf(...) normal_printf(__VA_ARGS__)
#endif

int main(int argc, char **argv) {
  printf("Don't you wish that it worked too?\n");
}
