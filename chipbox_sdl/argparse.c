#include "SDL.h"
#include "core.h"
#include "argparse.h"
#include <string.h>

/* parse and "execute" command line arguments, reading up to size_to_read bytes into file_data */
int handle_args(int argc, char *argv[], int size_to_read, byte file_data[]) {
    SDL_RWops *file = NULL;
    Sint64 file_size;

    if (argc < 2 || find_arg(argc, argv, "help") != -1) {
        printf("Please specify a ROM file\n");
        return 1;
    } else {
        file = SDL_RWFromFile(argv[argc - 1], "rb");
        if (file == NULL) {
            printf("File read error: %s\n", SDL_GetError());
            return 1;
        }
        file_size = SDL_RWsize(file);
        if (file_size < 0) {
            printf("File size detection error: %s\n", SDL_GetError());
            return 1;
        }
        size_to_read = (size_to_read < file_size) ? size_to_read : file_size; /* read no more than max size or size of file */
        SDL_RWread(file, file_data, sizeof(byte), size_to_read);
    }

    return 0;
}

/* find_arg: looks in command line arguments for short or long version of "string" as an argument
   e.g. if string is "help", long_arg is "--help" and short_arg is "-h"
   returns index of the first found matching argument, or -1 if none found */
int find_arg(int argc, char *argv[], char *string) {
    int index = -1;
    int i;
    char long_arg[MAX_LONG_ARG_LENGTH] = "--";
    char short_arg[MAX_SHORT_ARG_LENGTH] = "-";
    strcat(long_arg, string);
    short_arg[1] = string[0];

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], long_arg) == 0 || strcmp(argv[i], short_arg) == 0) {
            index = i;
            break;
        }
    }

    return index;
}
