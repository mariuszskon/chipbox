#include "SDL.h"
#include "chipbox_sdl.h"
#include "core.h"
#include "cpu.h"
#include "vm.h"
#include "render.h"
#include "audio.h"
#include "argparse.h"
#include "chipbox_sdl_argparse.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_AudioDeviceID audio_device;
    int scale = CHIPBOX_SDL_DEFAULT_SCALE;
    int tps = CHIPBOX_SDL_DEFAULT_TPS;
    int size_to_read = CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START;
    byte file_data[CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START];

    if (!handle_args(argc, argv, size_to_read, file_data, &scale, &tps) || !setup_sdl(&window, &renderer, &audio_device, scale)) {
        /* there was an error in handling command-line argumetns or in the initialisation of SDL */
        return 1;
    }

    run_chipbox(renderer, audio_device, scale, tps, file_data, size_to_read);

    quit_sdl(window, renderer, audio_device);
    return 0;
}

int setup_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_AudioDeviceID *audio_device, int scale) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("chipbox_sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CHIPBOX_SCREEN_WIDTH_PIXELS * scale, CHIPBOX_SCREEN_HEIGHT * scale, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        fprintf(stderr, "Window creation failure: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL) {
        fprintf(stderr, "Renderer creation failure: %s\n", SDL_GetError());
        return 0;
    }

    *audio_device = init_audio();
    if (audio_device == 0) {
        fprintf(stderr, "Audio device initialisation failure: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

int run_chipbox(SDL_Renderer *renderer, SDL_AudioDeviceID audio_device, int scale, int tps, byte file_data[], int size_to_read) {
    SDL_Event e;
    struct chipbox_chip8_state state;
    int pixel_count;
    SDL_Rect pixel_rects[CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT];
    unsigned long last_timer_change_time;
    unsigned long new_time;
    unsigned long current_time;
    unsigned long delta_time = 0;
    int running = 1;
    int i;
    int ticks_to_do;

    state = chipbox_init_state();
    chipbox_cpu_load_program(&state, file_data, size_to_read);
    last_timer_change_time = SDL_GetTicks();
    current_time = SDL_GetTicks();
    while (running) {
        new_time = SDL_GetTicks();
        delta_time += new_time - current_time;
        current_time = new_time;
        while(SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        chipbox_vm_update_input(&state);

        for (ticks_to_do = i = (delta_time * tps) / 1000; i > 0; i--) {
            if (!chipbox_vm_step(&state, &last_timer_change_time)) {
                running = 0;
                break;
            } else {
                handle_sound(audio_device, state.ST);
            }
        }
        delta_time -= (ticks_to_do * 1000) / tps; /* account for left over time */
        printf("%d\n", (ticks_to_do * 1000) / tps);

        chipbox_screen_to_sdl_rects(state.screen, pixel_rects, &pixel_count);
        chipbox_render(renderer, pixel_rects, pixel_count, scale);
    }

    return 0;
}

void quit_sdl(SDL_Window *window, SDL_Renderer *renderer, SDL_AudioDeviceID audio_device) {
    close_audio(audio_device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
