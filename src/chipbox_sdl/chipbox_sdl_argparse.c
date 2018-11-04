#include "SDL.h"
#include "core.h"
#include "log.h"
#include "argparse.h"
#include "chipbox_sdl_argparse.h"
#include "chipbox_sdl_config.h"
#include "chipbox_sdl_debug.h"
#include <stdio.h>

/* parse and "execute" command line arguments, reading up to size_to_read bytes into file_data */
int handle_args(int argc, char *argv[], int *size_to_read, byte file_data[], struct chipbox_sdl_config *config) {
    SDL_RWops *file = NULL;
    Sint64 file_size;
    int i;
    char long_arg[MAX_LONG_ARG_LENGTH+1];
    char short_arg[MAX_SHORT_ARG_LENGTH+1];
    char compat_str[MAX_COMPAT_LENGTH+1];
    char log_level_str[MAX_LOG_LEVEL_LENGTH+1];
    char debug_level_str[MAX_DEBUG_LEVEL_LENGTH+1];

    char args_list[CHIPBOX_SDL_ARG_NUM][MAX_LONG_ARG_LENGTH+1 - 2] = {
        "help",
        "scale",
        "tps",
        "mode",
        "log",
        "debug"
    };

    char args_helptext[CHIPBOX_SDL_ARG_NUM][MAX_HELPTEXT_LENGTH] = {
        "show this help message",
        "set scaling of display",
        "set CHIP-8 ticks/instructions per second",
        "set 'mattmik' or 'cowgod' compatibility mode",
        "set CPU logging level (none, error, warn (default), info)",
        "set debug mode (none (default), instruction, everything)"
    };

    if (argc < 2 || find_arg(argc, argv, "help") != -1) {
        printf("Usage: %s [options...] [file]\n", argv[0]);
        printf("\nOptions:\n");
        for (i = 0; i < CHIPBOX_SDL_ARG_NUM; i++) {
            string_to_short_long_args(args_list[i], short_arg, long_arg);
            printf(" %*s, %-*s %-*s\n", MAX_SHORT_ARG_LENGTH, short_arg, MAX_LONG_ARG_LENGTH, long_arg, MAX_HELPTEXT_LENGTH, args_helptext[i]);
        }
        return 0;
    }

    /* read the desired file */
    file = SDL_RWFromFile(argv[argc - 1], "rb");
    if (file == NULL) {
        fprintf(stderr, "File read error: %s\n", SDL_GetError());
        return 0;
    }
    argv[argc - 1] = NULL; /* do not interpret as argument */
    file_size = SDL_RWsize(file);
    if (file_size < 0) {
        fprintf(stderr, "File size detection error: %s\n", SDL_GetError());
        return 0;
    }
    *size_to_read = (*size_to_read < file_size) ? *size_to_read : file_size; /* read no more than max size or size of file */
    SDL_RWread(file, file_data, sizeof(byte), *size_to_read);
    SDL_RWclose(file);

    config->scale = get_int_arg_or_default(argc, argv, "scale", CHIPBOX_SDL_DEFAULT_SCALE);
    if (!nonzero_positive(config->scale, "scale")) return 0;
    config->tps = get_int_arg_or_default(argc, argv, "tps", CHIPBOX_SDL_DEFAULT_TPS);
    if (!nonzero_positive(config->tps, "tps")) return 0;

    get_str_arg_or_default(argc, argv, "mode", compat_str, MAX_COMPAT_LENGTH, CHIPBOX_SDL_DEFAULT_COMPAT_MODE_STR);
    if (strcmp(compat_str, "mattmik") == 0 || strcmp(compat_str, "MATTMIK") == 0) {
        config->compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    } else if (strcmp(compat_str, "cowgod") == 0 || strcmp(compat_str, "COWGOD") == 0) {
        config->compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    } else {
        fprintf(stderr, "Unrecognised compatiblity mode '%s'\n", compat_str);
        config->compat_mode = CHIPBOX_COMPATIBILITY_MODE_DEFAULT;
    }

    get_str_arg_or_default(argc, argv, "log", log_level_str, MAX_LOG_LEVEL_LENGTH, CHIPBOX_SDL_DEFAULT_LOG_LEVEL_STR);
    if (strcmp(log_level_str, "warn") == 0 || strcmp(log_level_str, "WARN") == 0 ||
        strcmp(log_level_str, "w") == 0 || strcmp(log_level_str, "W") == 0) {
        config->log_level = CHIPBOX_LOG_LEVEL_WARN;
    } else if (strcmp(log_level_str, "error") == 0 || strcmp(log_level_str, "ERROR") == 0 ||
               strcmp(log_level_str, "e") == 0 || strcmp(log_level_str, "E") == 0) {
        config->log_level = CHIPBOX_LOG_LEVEL_ERROR;
    } else if (strcmp(log_level_str, "info") == 0 || strcmp(log_level_str, "INFO") == 0 ||
               strcmp(log_level_str, "i") == 0 || strcmp(log_level_str, "I") == 0) {
        config->log_level = CHIPBOX_LOG_LEVEL_INFO;
    } else if (strcmp(log_level_str, "none") == 0 || strcmp(log_level_str, "NONE") == 0 ||
               strcmp(log_level_str, "n") == 0 || strcmp(log_level_str, "N") == 0) {
        config->log_level = CHIPBOX_LOG_LEVEL_NONE;
    } else {
        fprintf(stderr, "Unrecognised log level '%s'\n", log_level_str);
        config->log_level = CHIPBOX_LOG_LEVEL_DEFAULT;
    }

    get_str_arg_or_default(argc, argv, "debug", debug_level_str, MAX_DEBUG_LEVEL_LENGTH, CHIPBOX_SDL_DEFAULT_DEBUG_LEVEL_STR);
    if (strcmp(debug_level_str, "everything") == 0 || strcmp(log_level_str, "EVERYTHING") == 0 ||
        strcmp(debug_level_str, "e") == 0 || strcmp(log_level_str, "E") == 0) {
        config->debug_level = CHIPBOX_DEBUG_LEVEL_EVERYTHING;
    } else if (strcmp(debug_level_str, "instruction") == 0 || strcmp(debug_level_str, "INSTRUCTION") == 0 ||
               strcmp(debug_level_str, "i") == 0 || strcmp(debug_level_str, "i") == 0) {
        config->debug_level = CHIPBOX_DEBUG_LEVEL_INSTRUCTION;
    } else if (strcmp(debug_level_str, "none") == 0 || strcmp(debug_level_str, "NONE") == 0 ||
               strcmp(debug_level_str, "n") == 0 || strcmp(debug_level_str, "N") == 0) {
        config->debug_level = CHIPBOX_DEBUG_LEVEL_NONE;
    } else {
        fprintf(stderr, "Unrecognised debug level '%s'\n", debug_level_str);
        config->debug_level = CHIPBOX_DEBUG_LEVEL_DEFAULT;
    }

    if (unfound_args(argc, argv)) {
        return 0;
    }

    return 1;
}
