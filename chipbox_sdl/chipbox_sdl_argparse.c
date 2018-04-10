#include "SDL.h"
#include "core.h"
#include "argparse.h"
#include "chipbox_sdl_argparse.h"

/* parse and "execute" command line arguments, reading up to size_to_read bytes into file_data */
int handle_args(int argc, char *argv[], int size_to_read, byte file_data[]) {
    SDL_RWops *file = NULL;
    Sint64 file_size;

    if (argc < 2 || find_arg(argc, argv, "help") != -1) {
        printf("Usage: %s [options...] <file>\n", argv[0]);
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
