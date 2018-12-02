#include "SDL.h"
#include "chipbox_sdl.h"
#include "core.h"
#include "cpu.h"
#include "vm.h"
#include "render.h"
#include "audio.h"
#include "argparse.h"
#include "chipbox_sdl_argparse.h"
#include "chipbox_sdl_config.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* chip8_screen = NULL;
    SDL_AudioDeviceID audio_device;
    struct chipbox_sdl_config config;
    int size_to_read = CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START;
    byte file_data[CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START];
    byte play_sound = 0; /* boolean for whether a sound should be playing now or not */
    config.scale = CHIPBOX_SDL_DEFAULT_SCALE;
    config.tps = CHIPBOX_SDL_DEFAULT_TPS;
    config.compat_mode = CHIPBOX_COMPATIBILITY_MODE_DEFAULT;

    if (!handle_args(argc, argv, &size_to_read, file_data, &config) || !setup_sdl(&window, &renderer, &chip8_screen, &audio_device, &play_sound, &config)) {
        /* there was an error in handling command-line argumetns or in the initialisation of SDL */
        return 1;
    }

    run_chipbox(renderer, chip8_screen, &play_sound, file_data, size_to_read, &config);

    quit_sdl(window, renderer, chip8_screen, audio_device);
    return 0;
}

int setup_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture** chip8_screen, SDL_AudioDeviceID *audio_device, byte *play_sound, struct chipbox_sdl_config *config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 0;
    }

    if (config->blur) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    } else {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    }

    *window = SDL_CreateWindow("chipbox_sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CHIPBOX_SCREEN_WIDTH_PIXELS * config->scale, CHIPBOX_SCREEN_HEIGHT * config->scale, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        fprintf(stderr, "Window creation failure: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL) {
        fprintf(stderr, "Renderer creation failure: %s\n", SDL_GetError());
        return 0;
    }
    *chip8_screen = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CHIPBOX_SCREEN_WIDTH_PIXELS, CHIPBOX_SCREEN_HEIGHT);
    SDL_SetRenderTarget(*renderer, *chip8_screen);
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(*renderer);
    SDL_SetRenderTarget(*renderer, NULL);
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(*renderer);

    *audio_device = init_audio(play_sound);
    if (audio_device == 0) {
        fprintf(stderr, "Audio device initialisation failure: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

int run_chipbox(SDL_Renderer *renderer, SDL_Texture *chip8_screen, byte *play_sound, byte file_data[], int size_to_read, struct chipbox_sdl_config *config) {
    SDL_Event e;
    struct chipbox_chip8_state state;
    int pixel_count;
    SDL_Rect pixel_rects[CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT];
    unsigned long new_time;
    unsigned long current_time;
    unsigned long delta_time = 0;
    int running = 1;
    int paused = 0;
    int i;
    int ticks_to_do;

    state = chipbox_init_state(config->tps);
    chipbox_cpu_srand(&state, config->seed);
    state.compat_mode = config->compat_mode;
    chipbox_cpu_load_program(&state, file_data, size_to_read);
    current_time = SDL_GetTicks();
    while (running) {
        new_time = SDL_GetTicks();
        delta_time += new_time - current_time;
        current_time = new_time;
        while(SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) { /* ensure keys used for debugging are not registered when held down to prevent very fast toggling */
                if (e.key.keysym.sym == SDLK_p) {
                    paused = !paused;
                }
            }
        }

        if (!paused) {
            chipbox_vm_update_input(&state);

            for (ticks_to_do = i = (delta_time * config->tps) / 1000; i > 0; i--) {
                if (!chipbox_vm_step(&state, config)) {
                    running = 0;
                    break;
                } else {
                    *play_sound = handle_sound(state.ST);
                }
            }
            delta_time -= (ticks_to_do * 1000) / config->tps; /* account for left over time */

            chipbox_screen_to_sdl_rects(state.screen, pixel_rects, &pixel_count);
            chipbox_render(renderer, chip8_screen, pixel_rects, pixel_count, config->ghosting);
        } else {
            /* do not account for change in time if paused to prevent speed up when unpausing */
            delta_time = 0;
            current_time = SDL_GetTicks();
        }
    }

    return 0;
}

void quit_sdl(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *chip8_screen, SDL_AudioDeviceID audio_device) {
    close_audio(audio_device);
    SDL_DestroyTexture(chip8_screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
