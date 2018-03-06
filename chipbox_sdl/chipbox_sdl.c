#include "chipbox_sdl.h"
#include "vm.h"
#include "render.h"
#include <stdio.h>


int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event e;
    struct chipbox_chip8_state state;
    int pixel_count;
    SDL_Rect pixel_rects[CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT];
    int scale = 8;
    SDL_RWops *file = NULL;
    Sint64 file_size;
    int size_to_read = CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START;
    byte file_data[CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START];
    unsigned long last_timer_change_time;
    unsigned long last_time;
    int ticks;
    int ticks_per_second = 500;
    int ms_per_tick = 1000 / ticks_per_second;
    int running = 1;

    if (argc < 2) {
        printf("Please specify a ROM file\n");
        return 1;
    } else {
        file = SDL_RWFromFile(argv[argc - 1], "rb");
        if (file == NULL) {
            printf("File read error: %s\n", SDL_GetError());
            return 1;
        }
        file_size = SDL_RWsize(file);
        if (file_size < 0) {
            printf("File size detection error: %s\n", SDL_GetError());
            return 1;
        }
        size_to_read = (size_to_read < file_size) ? size_to_read : file_size; /* read no more than max size or size of file */
        SDL_RWread(file, file_data, sizeof(byte), size_to_read);
    }

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
    chipbox_cpu_load_program(&state, file_data, size_to_read);
    last_timer_change_time = SDL_GetTicks();
    while (running) {
        while(SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        chipbox_vm_update_input(&state);

        ticks = 0;
        do {
            if (!chipbox_vm_step(&state, &last_timer_change_time)) {
                running = 0;
                break;
            }
            chipbox_screen_to_sdl_rects(state.screen, pixel_rects, &pixel_count);
            chipbox_render(renderer, pixel_rects, pixel_count, CHIPBOX_SCREEN_WIDTH_PIXELS, CHIPBOX_SCREEN_HEIGHT, scale);
            ticks++;
            last_time = SDL_GetTicks();
            while (SDL_GetTicks() - last_time < ms_per_tick); /* busy wait because sleep does not have enough precision */
        } while (ticks < ticks_per_second);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
