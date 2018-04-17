#include "argparse.h"
#include <stdio.h>
#include <string.h>

/* find_arg: looks in command line arguments for short or long version of "string" as an argument
   e.g. if string is "help", long_arg is "--help" and short_arg is "-h"
   returns index of the first found matching argument, or -1 if none found */
int find_arg(int argc, char *argv[], char *string) {
    int index = -1;
    int i;
    char long_arg[MAX_LONG_ARG_LENGTH+1];
    char short_arg[MAX_SHORT_ARG_LENGTH+1];
    string_to_short_long_args(string, short_arg, long_arg);

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], long_arg) == 0 || strcmp(argv[i], short_arg) == 0) {
            index = i;
            break;
        }
    }

    return index;
}

void string_to_short_long_args(char *string, char *short_arg, char *long_arg) {
    strcpy(long_arg, "--");
    short_arg[0] = '-';
    strcat(long_arg, string);
    short_arg[1] = string[0];
    short_arg[2] = '\0';
}

int get_int_arg_or_default(int argc, char *argv[], char *string, int default_value) {
    int index = find_arg(argc, argv, string);
    int result;

    if (index == -1) {
        return default_value;
    }

    /* do not try to read next argument if we are already on the last one */
    if (index > argc - 1 - 1) {
        return default_value;
    }

    if (sscanf(argv[index + 1], "%d", &result) != 1) {
        fprintf(stderr, "Argument error: '%s' could not be converted to an integer\n", argv[index + 1]);
        return default_value;
    }

    return result;
}

int nonzero_positive(int x, char *name) {
    if (x < 1) {
        fprintf(stderr, "Argument error: must set %s to a positive non-zero integer\n", name);
        return 0;
    } else {
        return 1;
    }
}
