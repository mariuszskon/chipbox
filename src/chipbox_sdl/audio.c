#include "SDL.h"
#include "audio.h"
#include "core.h"

void generate_beep(void *userdata, Uint8 *stream, int len) {
    int i;
    byte *play_sound = (byte *)(userdata);

    if (*play_sound) {
        for (i = 0; i < len; i++) {
            stream[i] = (i % CHIPBOX_BEEP_PERIOD < CHIPBOX_BEEP_PERIOD / 2) * CHIPBOX_BEEP_AMPLITUDE;
        }
    } else {
        /* fill with silence */
        for (i = 0; i < len; i++) {
            stream[i] = 0;
        }
    }
}

SDL_AudioDeviceID init_audio(byte *play_sound) {
    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = CHIPBOX_BEEP_FREQUENCY;
    want.format = AUDIO_U8;
    want.channels = CHIPBOX_BEEP_CHANNELS;
    want.samples = CHIPBOX_BEEP_SAMPLES;
    want.callback = generate_beep;
    /* provide a pointer to a boolean (actual type "byte")
       for whether a sound should be played or not */
    want.userdata = play_sound;

    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_PauseAudioDevice(dev, 0); /* play */
    return dev;
}

void close_audio(SDL_AudioDeviceID device) {
    SDL_CloseAudioDevice(device);
}

byte handle_sound(byte st) {
    return st >= CHIPBOX_MINIMUM_ST_FOR_BEEP;
}

