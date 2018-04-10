#ifndef CHIPBOX_ARGPARSE_H
#define CHIPBOX_ARGPARSE_H

/* max length of long form and short form of arguments, not including null terminator */
#define MAX_LONG_ARG_LENGTH 16
#define MAX_SHORT_ARG_LENGTH 2

int find_arg(int argc, char *argv[], char *string);
void string_to_short_long_args(char *string, char *short_arg, char *long_arg);
int get_int_arg_or_default(int argc, char *argv[], char *string, int default_value);

#endif
