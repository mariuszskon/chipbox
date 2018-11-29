#ifndef CHIPBOX_SDL_CONFIG_H
#define CHIPBOX_SDL_CONFIG_H

/* a convenient struct for variables can than be set at runtime for configuring
   the behaviour of chipbox_sdl */
struct chipbox_sdl_config {
    int scale; /* how much in both horizontal and vertical direction pixels should be stretched */
    byte blur; /* if blurring through linear filtering should be applied or not */
    int tps; /* ticks/instructions per second */
    byte compat_mode; /* cpu compatibility mode (e.g. mattmik or cowgod) */
    byte log_level; /* log messages must be CHIPBOX_LOG_LEVEL_* or lower to be printed */
    byte debug_level; /* what debug messages should be shown, must be CHIPBOX_DEBUG_LEVEL_* */
    dbyte seed; /* seed to use for random number generation */
    int ghosting;
};

#endif
