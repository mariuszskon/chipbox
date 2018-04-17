#ifndef CHIPBOX_SDL_ARGPARSE_H
#define CHIPBOX_SDL_ARGPARSE_H

#define CHIPBOX_SDL_ARG_NUM 3
#define MAX_HELPTEXT_LENGTH 64
#define CHIPBOX_SDL_DEFAULT_SCALE 8
#define CHIPBOX_SDL_DEFAULT_TPS 500

int handle_args(int argc, char *argv[], int size_to_read, byte file_data[], int *scale, int *tps);

#endif
