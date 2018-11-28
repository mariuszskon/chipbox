#ifndef CHIPBOX_RENDER_H
#define CHIPBOX_RENDER_H

#include "core.h"

void chipbox_screen_to_sdl_rects(byte screen[], SDL_Rect rects[], int *count);
void chipbox_render(SDL_Renderer* renderer, SDL_Texture *chip8_screen, SDL_Rect rects[], int count);

#endif
