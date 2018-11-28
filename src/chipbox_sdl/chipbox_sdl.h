#ifndef CHIPBOX_SDL_H
#define CHIPBOX_SDL_H

#include "SDL.h"
#include "core.h"
#include "chipbox_sdl_config.h"

int setup_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture** chip8_screen, SDL_AudioDeviceID *audio_device, byte *play_sound, struct chipbox_sdl_config *config);
int run_chipbox(SDL_Renderer *renderer, SDL_Texture *chip8_screen, byte *play_sound, byte file_data[], int size_to_read, struct chipbox_sdl_config *config);
void quit_sdl(SDL_Window *window, SDL_Renderer *renderer, SDL_AudioDeviceID audio_device);

#endif
