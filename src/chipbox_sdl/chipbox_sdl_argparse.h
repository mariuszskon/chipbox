#ifndef CHIPBOX_SDL_ARGPARSE_H
#define CHIPBOX_SDL_ARGPARSE_H

#include <time.h>

#define CHIPBOX_SDL_ARG_NUM 7
#define MAX_HELPTEXT_LENGTH 64
#define MAX_COMPAT_LENGTH 7
#define MAX_LOG_LEVEL_LENGTH 5
#define MAX_DEBUG_LEVEL_LENGTH 11
#define CHIPBOX_SDL_DEFAULT_SCALE 8
#define CHIPBOX_SDL_DEFAULT_TPS 500
#define CHIPBOX_SDL_DEFAULT_COMPAT_MODE_STR "MATTMIK"
#define CHIPBOX_SDL_DEFAULT_LOG_LEVEL_STR "WARN"
#define CHIPBOX_SDL_DEFAULT_DEBUG_LEVEL_STR "NONE"
#define CHIPBOX_SDL_DEFAULT_SEED time(NULL)

#include "chipbox_sdl_config.h"

int handle_args(int argc, char *argv[], int *size_to_read, byte file_data[], struct chipbox_sdl_config *config);

#endif
