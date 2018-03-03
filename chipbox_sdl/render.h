#ifndef CHIPBOX_RENDER_H
#define CHIPBOX_RENDER_H

#include "chipbox_sdl.h"

void chipbox_screen_to_sdl_rects(byte screen[], SDL_Rect rects[], int *count);
void chipbox_render(SDL_Renderer* renderer, SDL_Rect rects[], int count, int width, int height, int scale);

#endif
