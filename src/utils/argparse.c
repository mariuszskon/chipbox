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
        if (argv[i] != NULL && (strcmp(argv[i], long_arg) == 0 || strcmp(argv[i], short_arg) == 0)) {
            index = i;
            argv[i] = NULL; /* mark argument as 'found' */
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

/* sees if it is safe to interpret a "--argument parameter" pair, returning -1 index if not */
int validate_arg_with_parameter(int argc, char *argv[], char *string) {
    int index = find_arg(argc, argv, string);

    /* already interpreted as an argument elsewhere */
    if (argv[index + 1] == NULL) {
        fprintf(stderr, "Argument error: no parameter for %s provided\n", string);
        return -1;
    }

    return index;
}

int get_int_arg_or_default(int argc, char *argv[], char *string, int default_value) {
    int index = validate_arg_with_parameter(argc, argv, string);
    int result;

    if (index == -1) {
        return default_value;
    }

    if (sscanf(argv[index + 1], "%d", &result) != 1) {
        fprintf(stderr, "Argument error: '%s' could not be converted to an integer\n", argv[index + 1]);
        argv[index + 1] = NULL;
        return default_value;
    }

    argv[index + 1] = NULL;

    return result;
}

/* writes at most "dest_len" characters + null terminator to "dest" from
   either parameter to argument "string", or "default_value" */
int get_str_arg_or_default(int argc, char *argv[], char *string, char *dest, int dest_len, char *default_value) {
    int index = validate_arg_with_parameter(argc, argv, string);
    int input_len;
    dest[0] = '\0';

    if (index == -1) {
        strncat(dest, default_value, dest_len);
        return 0;
    } else {
        input_len = strlen(argv[index + 1]);
        if (input_len > dest_len) {
            fprintf(stderr, "Argument error: '%s' is too long of a parameter for %s\n", argv[index + 1], string);
            strncat(dest, default_value, dest_len);
            argv[index + 1] = NULL;
            return 0;
        }
        strncat(dest, argv[index + 1], dest_len);
        argv[index + 1] = NULL;
        return 1;
    }
}

int nonzero_positive(int x, char *name) {
    if (x < 1) {
        fprintf(stderr, "Argument error: must set %s to a positive non-zero integer\n", name);
        return 0;
    } else {
        return 1;
    }
}

int unfound_args(int argc, char *argv[]) {
    int unfound = 0;
    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i] != NULL) {
            fprintf(stderr, "Argument error: unknown option '%s'\n", argv[i]);
            unfound = 1;
        }
    }

    return unfound;
}
