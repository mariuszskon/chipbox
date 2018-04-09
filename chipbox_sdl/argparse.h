#ifndef CHIPBOX_ARGPARSE_H
#define CHIPBOX_ARGPARSE_H

#include "core.h"

#define MAX_LONG_ARG_LENGTH 256
#define MAX_SHORT_ARG_LENGTH 8

int handle_args(int argc, char *argv[], int size_to_read, byte file_data[]);
int find_arg(int argc, char *argv[], char *string);
void string_to_short_long_args(char *string, char *short_arg, char *long_arg);

#endif
