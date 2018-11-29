#include "SDL.h"
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

/* chipbox_render: render chip8 screen, scaling each pixel by scale */
void chipbox_render(SDL_Renderer* renderer, SDL_Texture *chip8_screen, SDL_Rect rects[], int count, int ghosting) {
    if (ghosting < 0 || ghosting > 0xFF) ghosting = 1;
    SDL_SetRenderTarget(renderer, chip8_screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF / ghosting);
    SDL_RenderFillRect(renderer, NULL);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRects(renderer, rects, count);

    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, chip8_screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}
