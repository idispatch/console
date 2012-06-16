#include "font.h"
#include "console.h"

#ifdef CONSOLE_USE_FONT_4x6
extern unsigned char console_font_4x6[];
#endif

#ifdef CONSOLE_USE_FONT_4x7
extern unsigned char console_font_4x7[];
#endif

#ifdef CONSOLE_USE_FONT_5x8
extern unsigned char console_font_5x8[];
#endif

#ifdef CONSOLE_USE_FONT_5x12
extern unsigned char console_font_5x12[];
#endif

#ifdef CONSOLE_USE_FONT_6x8
extern unsigned char console_font_6x8[];
#endif

#ifdef CONSOLE_USE_FONT_7x9
extern unsigned char console_font_7x9[];
#endif

#ifdef CONSOLE_USE_FONT_8x8
extern unsigned char console_font_8x8[];
#endif

#ifdef CONSOLE_USE_FONT_8x10
extern unsigned char console_font_8x10[];
#endif

#ifdef CONSOLE_USE_FONT_8x16
extern unsigned char console_font_8x16[];
#endif

#ifdef CONSOLE_USE_FONT_9x8
extern unsigned char console_font_9x8[];
#endif

#ifdef CONSOLE_USE_FONT_9x16
extern unsigned char console_font_9x16[];
#endif

#ifdef CONSOLE_USE_FONT_10x20
extern unsigned char console_font_10x20[];
#endif

#ifdef CONSOLE_USE_FONT_12x16
extern unsigned char console_font_12x16[];
#endif

#ifdef CONSOLE_USE_FONT_12x23
extern unsigned char console_font_12x23[];
#endif

#ifdef CONSOLE_USE_FONT_12x24
extern unsigned char console_font_12x24[];
#endif

#ifdef CONSOLE_USE_FONT_12x27
extern unsigned char console_font_12x27[];
#endif

#ifdef CONSOLE_USE_FONT_14x30
extern unsigned char console_font_14x30[];
#endif

#ifdef CONSOLE_USE_FONT_16x32
extern unsigned char console_font_16x32[];
#endif

#ifdef CONSOLE_USE_FONT_16x37
extern unsigned char console_font_16x37[];
#endif

#ifdef CONSOLE_USE_FONT_25x57
extern unsigned char console_font_25x57[];
#endif

const font_t console_fonts[] = {
#ifdef CONSOLE_USE_FONT_4x6
    [FONT_4x6] = {
        .char_width = 4,
        .char_height = 6,
        .font_name = "4x6",
        .font_bitmap = &console_font_4x6[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_4x7
    [FONT_4x7] = {
        .char_width = 4,
        .char_height = 7,
        .font_name = "4x7",
        .font_bitmap = &console_font_4x7[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_5x8
    [FONT_5x8] = {
        .char_width = 5,
        .char_height = 8,
        .font_name = "5x8",
        .font_bitmap = &console_font_5x8[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_5x12
    [FONT_5x12] = {
        .char_width = 5,
        .char_height = 12,
        .font_name = "5x12",
        .font_bitmap = &console_font_5x12[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_6x8
    [FONT_6x8] = {
        .char_width = 6,
        .char_height = 8,
        .font_name = "6x8",
        .font_bitmap = &console_font_6x8[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_7x9
    [FONT_7x9] = {
        .char_width = 7,
        .char_height = 9,
        .font_name = "7x9",
        .font_bitmap = &console_font_7x9[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_8x8
    [FONT_8x8] = {
        .char_width = 8,
        .char_height = 8,
        .font_name = "System (8x8)",
        .font_bitmap = &console_font_8x8[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_8x10
    [FONT_8x10] = {
        .char_width = 8,
        .char_height = 10,
        .font_name = "System (8x10)",
        .font_bitmap = &console_font_8x10[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_8x16
    [FONT_8x16] = {
        .char_width = 8,
        .char_height = 16,
        .font_name = "System (8x16)",
        .font_bitmap = &console_font_8x16[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_9x8
    [FONT_9x8] = {
        .char_width = 9,
        .char_height = 8,
        .font_name = "9x8",
        .font_bitmap = &console_font_9x8[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_9x16
    [FONT_9x16] = {
        .char_width = 9,
        .char_height = 16,
        .font_name = "9x16",
        .font_bitmap = &console_font_9x16[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_10x20
    [FONT_10x20] = {
        .char_width = 10,
        .char_height = 20,
        .font_name = "10x20",
        .font_bitmap = &console_font_10x20[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_12x16
    [FONT_12x16] = {
        .char_width = 12,
        .char_height = 16,
        .font_name = "12x16",
        .font_bitmap = &console_font_12x16[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_12x23
    [FONT_12x23] = {
        .char_width = 12,
        .char_height = 23,
        .font_name = "12x23",
        .font_bitmap = &console_font_12x23[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_12x24
    [FONT_12x24] = {
        .char_width = 12,
        .char_height = 24,
        .font_name = "12x24",
        .font_bitmap = &console_font_12x24[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_12x27
    [FONT_12x27] = {
        .char_width = 12,
        .char_height = 27,
        .font_name = "12x27",
        .font_bitmap = &console_font_12x27[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_14x30
    [FONT_14x30] = {
        .char_width = 14,
        .char_height = 30,
        .font_name = "14x30",
        .font_bitmap = &console_font_14x30[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_16x32
    [FONT_16x32] = {
        .char_width = 16,
        .char_height = 32,
        .font_name = "16x32",
        .font_bitmap = &console_font_16x32[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_16x37
    [FONT_16x37] = {
        .char_width = 16,
        .char_height = 37,
        .font_name = "16x37",
        .font_bitmap = &console_font_16x37[0]
    },
#endif
#ifdef CONSOLE_USE_FONT_25x57
    [FONT_25x57] = {
        .char_width = 25,
        .char_height = 57,
        .font_name = "25x57",
        .font_bitmap = &console_font_25x57[0]
    },
#endif
};
