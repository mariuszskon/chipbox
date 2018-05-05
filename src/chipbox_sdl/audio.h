#ifndef CHIPBOX_AUDIO_H
#define CHIPBOX_AUDIO_H

#include "SDL.h"
#include "core.h"

#define CHIPBOX_BEEP_FREQUENCY 44100 /* for the CD quality beep */
#define CHIPBOX_BEEP_SAMPLES 256 /* number of SDL sound samples */
#define CHIPBOX_BEEP_CHANNELS 1

#define CHIPBOX_BEEP_PERIOD 256
#define CHIPBOX_BEEP_AMPLITUDE 32

#define CHIPBOX_MINIMUM_ST_FOR_BEEP 2

void generate_beep(void *userdata, Uint8 *stream, int len);
SDL_AudioDeviceID init_audio();
void close_audio(SDL_AudioDeviceID device);
byte handle_sound(byte st);

#endif
