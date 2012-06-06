#include "font.h"
#include "console.h"

extern unsigned char console_font_8x8[];
extern unsigned char console_font_8x10[];
extern unsigned char console_font_8x16[];

const font_t console_fonts[] = {
    [FONT_8x8_SYSTEM] = {
        .char_width = 8,
        .char_height = 8,
        .font_name = "System (8x8)",
        .first_char = 0,
        .last_char = 255,
        .font_bitmap = &console_font_8x8[0]
    },
    [FONT_8x10] = {
        .char_width = 8,
        .char_height = 10,
        .font_name = "System (8x10)",
        .first_char = 0,
        .last_char = 255,
        .font_bitmap = &console_font_8x10[0]
    },
    [FONT_8x16] = {
        .char_width = 8,
        .char_height = 16,
        .font_name = "System (8x16)",
        .first_char = 0,
        .last_char = 255,
        .font_bitmap = &console_font_8x16[0]
    }
};

