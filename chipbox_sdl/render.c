#include "render.h"

/* chipbox_screen_to_sdl_rects: takes in screen data (from chipbox_chip8_state) and write on pixels as SDL_Rect to rects[],
   setting count to amount of on pixels. rects[] must be at least CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT */
void chipbox_screen_to_sdl_rects(byte screen[], SDL_Rect rects[], int *count) {
    count = 0;
}

/* chipbox_render: render chip8 screen to window of size width * scale by height * scale */
void chipbox_render(SDL_Renderer* renderer, SDL_Rect rects[], int count, int width, int height, int scale) {
    SDL_Rect background;
    background.x = background.y = 0;
    background.w = width * scale;
    background.h = height * scale;
    SDL_RenderClear(renderer);
    SDL_RenderDrawRect(renderer, &background);
    SDL_RenderDrawRects(renderer, rects, count);
    SDL_RenderPresent(renderer);
}
