#include "chipbox_sdl.h"
#include <stdio.h>

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("chipbox_sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window creation failure: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
