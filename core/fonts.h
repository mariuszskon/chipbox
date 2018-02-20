#ifndef CHIPBOX_FONTS_H
#define CHIPBOX_FONTS_H

#include "core.h"

#define CHIPBOX_FONT_SIZE 5 /* how many bytes/rows a font occupies */
#define CHIPBOX_FONT_CHARS 16 /* how many characters are in the font */
#define CHIPBOX_FONT_TOTAL_BYTES (CHIPBOX_FONT_SIZE * CHIPBOX_FONT_CHARS)

/* the font, where each code line is a character from 0-F */
extern const byte chipbox_font[CHIPBOX_FONT_TOTAL_BYTES];
#endif
