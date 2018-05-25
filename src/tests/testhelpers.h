#ifndef CHIPBOX_TESTHELPERS_H
#define CHIPBOX_TESTHELPERS_H

#include "core.h"

extern int tests;
extern int failed;

void test(int condition, char* name);
int all_equal(byte array[], int size, int value);
void print_section(int section_num);

#endif
