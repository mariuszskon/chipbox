#include "render.h"

/* chipbox_screen_to_sdl_rects: takes in screen data (from chipbox_chip8_state) and write on pixels as SDL_Rect to rects[],
   setting count to amount of on pixels. rects[] must be at least CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT */
void chipbox_screen_to_sdl_rects(byte screen[], SDL_Rect rects[], int *count) {
    int row, pixel_col, pixel_on;
    *count = 0;
    for (row = 0; row < CHIPBOX_SCREEN_HEIGHT; row++) {
        for (pixel_col = 0; pixel_col < CHIPBOX_SCREEN_WIDTH_PIXELS; pixel_col++) {
            /* this is a big one - get the respective BYTE of the screen, and AND the pixel we want
               if the result in >0, the pixel is on! */
            pixel_on = screen[CHIPBOX_SCREEN_WIDTH_BYTES * row + (pixel_col / 8)] & (0x80 >> (pixel_col % 8));
            if (pixel_on) {
                rects[*count].x = pixel_col;
                rects[*count].y = row;
                rects[*count].w = rects[*count].h = 1;
                (*count)++;
            }
        }
    }
}

/* chipbox_render: render chip8 screen to window of size width * scale by height * scale */
void chipbox_render(SDL_Renderer* renderer, SDL_Rect rects[], int count, int width, int height, int scale) {
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, rects, count);
    SDL_RenderPresent(renderer);
}
