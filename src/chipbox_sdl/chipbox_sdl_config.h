#ifndef CHIPBOX_SDL_CONFIG_H
#define CHIPBOX_SDL_CONFIG_H

/* a convenient struct for variables can than be set at runtime for configuring
   the behaviour of chipbox_sdl */
struct chipbox_sdl_config {
    int scale; /* how much in both horizontal and vertical direction pixels should be stretched */
    int tps; /* ticks/instructions per second */
    byte compat_mode; /* cpu compatibility mode (e.g. mattmik or cowgod) */
};

#endif
