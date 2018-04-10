#ifndef CHIPBOX_SDL_ARGPARSE_H
#define CHIPBOX_SDL_ARGPARSE_H

#define CHIPBOX_SDL_ARG_NUM 1
#define MAX_HELPTEXT_LENGTH 64

int handle_args(int argc, char *argv[], int size_to_read, byte file_data[]);

#endif
