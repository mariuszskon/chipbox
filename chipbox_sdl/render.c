#include "render.h"

/* chipbox_screen_to_sdl_rects: takes in screen data (from chipbox_chip8_state) and write on pixels as SDL_Rect to rects[],
   setting count to amount of on pixels. rects[] must be at least CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT */
void chipbox_screen_to_sdl_rects(byte screen[], SDL_Rect rects[], int *count) {
    
}

/* chipbox_render: render chip8 screen to window */
void chipbox_render(SDL_Renderer* renderer, SDL_Rect rects[], int count, int scale) {
    
}
