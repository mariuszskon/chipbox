#ifndef CHIPBOX_SDL_H
#define CHIPBOX_SDL_H

#include "SDL.h"

int setup_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_AudioDeviceID *audio_device, int scale);
void quit_sdl(SDL_Window *window, SDL_Renderer *renderer, SDL_AudioDeviceID audio_device);

#endif
