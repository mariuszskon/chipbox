#include "chipbox_sdl.h"
#include "vm.h"
#include "render.h"
#include <stdio.h>


int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event e;
    struct chipbox_chip8_state state;
    int pixel_count;
    SDL_Rect pixel_rects[CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT];
    int scale = 8;
    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("chipbox_sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CHIPBOX_SCREEN_WIDTH_PIXELS * scale, CHIPBOX_SCREEN_HEIGHT * scale, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window creation failure: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer creation failure: %s\n", SDL_GetError());
    }

    state = chipbox_init_state();
    while (running) {
        while(SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        if (!chipbox_vm_step(&state)) {
            running = 0;
        }
        chipbox_screen_to_sdl_rects(state.memory, pixel_rects, &pixel_count);
        chipbox_render(renderer, pixel_rects, pixel_count, CHIPBOX_SCREEN_WIDTH_PIXELS, CHIPBOX_SCREEN_HEIGHT, scale);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
