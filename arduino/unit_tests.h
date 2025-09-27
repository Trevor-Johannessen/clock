#ifndef UNITTESTS_H
#define UNITTESTS_H

#include "scheduler.h"

void test_run_long(char *name, long a, long b);
void test_run_double(char *name, double a, double b);
void test_run_str(char *name, char *a, char *b);
void test_scheduler();

#endif