#ifndef CHIPBOX_SDL_H
#define CHIPBOX_SDL_H

#include "SDL.h"
#include "core.h"

int setup_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_AudioDeviceID *audio_device, int scale);
int run_chipbox(SDL_Renderer *renderer, SDL_AudioDeviceID audio_device, int scale, byte file_data[], int size_to_read);
void quit_sdl(SDL_Window *window, SDL_Renderer *renderer, SDL_AudioDeviceID audio_device);

#endif
