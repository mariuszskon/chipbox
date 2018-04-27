#ifndef CHIPBOX_SDL_ARGPARSE_H
#define CHIPBOX_SDL_ARGPARSE_H

#define CHIPBOX_SDL_ARG_NUM 4
#define MAX_HELPTEXT_LENGTH 64
#define MAX_COMPAT_LENGTH 7
#define CHIPBOX_SDL_DEFAULT_SCALE 8
#define CHIPBOX_SDL_DEFAULT_TPS 500

#include "chipbox_sdl_config.h"

int handle_args(int argc, char *argv[], int size_to_read, byte file_data[], struct chipbox_sdl_config *config);

#endif
