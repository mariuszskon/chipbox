#include "SDL.h"
#include "audio.h"
#include "core.h"

void generate_beep(void *userdata, Uint8 *stream, int len) {
    int i;
    (void)(userdata); /* unused but required to be in function signature for SDL */

    for (i = 0; i < len; i++) {
        stream[i] = (i % CHIPBOX_BEEP_PERIOD < CHIPBOX_BEEP_PERIOD / 2) * CHIPBOX_BEEP_AMPLITUDE;
    }
}

SDL_AudioDeviceID init_audio() {
    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = CHIPBOX_BEEP_FREQUENCY;
    want.format = AUDIO_U8;
    want.channels = CHIPBOX_BEEP_CHANNELS;
    want.samples = CHIPBOX_BEEP_SAMPLES;
    want.callback = generate_beep;

    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    return dev;
}

void close_audio(SDL_AudioDeviceID device) {
    SDL_CloseAudioDevice(device);
}

void handle_sound(SDL_AudioDeviceID device, byte st) {
    if (st >= CHIPBOX_MINIMUM_ST_FOR_BEEP) {
        SDL_PauseAudioDevice(device, 0); /* play */
    } else {
        SDL_PauseAudioDevice(device, 1); /* pause */
    }
}

