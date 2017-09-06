#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <stdlib.h>
#include "common.h"

typedef struct watchpoint {
  char exp[64];
  int value;
  int NO;
  struct watchpoint *next;
} WP;

WP* set_wp(char* args);
bool del_wp(char* args);
WP* new_wp();
int free_wp(WP* wp);
int print_wp();
bool check_wp();

#endif
