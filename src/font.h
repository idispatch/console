#ifndef FONT_H_
#define FONT_H_

#if defined(CONSOLE_USE_ALL_FONTS) || ( \
    !defined(CONSOLE_USE_FONT_4x6) && \
    !defined(CONSOLE_USE_FONT_4x7) && \
    !defined(CONSOLE_USE_FONT_5x8) && \
    !defined(CONSOLE_USE_FONT_5x12) && \
    !defined(CONSOLE_USE_FONT_6x8) && \
    !defined(CONSOLE_USE_FONT_7x9) && \
    !defined(CONSOLE_USE_FONT_8x8) && \
    !defined(CONSOLE_USE_FONT_8x10) && \
    !defined(CONSOLE_USE_FONT_8x16) && \
    !defined(CONSOLE_USE_FONT_9x8) && \
    !defined(CONSOLE_USE_FONT_9x16) && \
    !defined(CONSOLE_USE_FONT_10x20) && \
    !defined(CONSOLE_USE_FONT_12x16) && \
    !defined(CONSOLE_USE_FONT_12x23) && \
    !defined(CONSOLE_USE_FONT_12x24) && \
    !defined(CONSOLE_USE_FONT_12x27) && \
    !defined(CONSOLE_USE_FONT_14x30) && \
    !defined(CONSOLE_USE_FONT_16x32) && \
    !defined(CONSOLE_USE_FONT_16x37) && \
    !defined(CONSOLE_USE_FONT_25x57))
#define CONSOLE_USE_FONT_4x6 1
#define CONSOLE_USE_FONT_4x7 1
#define CONSOLE_USE_FONT_5x8 1
#define CONSOLE_USE_FONT_5x12 1
#define CONSOLE_USE_FONT_6x8 1
#define CONSOLE_USE_FONT_7x9 1
#define CONSOLE_USE_FONT_8x8 1
#define CONSOLE_USE_FONT_8x10 1
#define CONSOLE_USE_FONT_8x16 1
#define CONSOLE_USE_FONT_9x8 1
#define CONSOLE_USE_FONT_9x16 1
#define CONSOLE_USE_FONT_10x20 1
#define CONSOLE_USE_FONT_12x16 1
#define CONSOLE_USE_FONT_12x23 1
#define CONSOLE_USE_FONT_12x24 1
#define CONSOLE_USE_FONT_12x27 1
#define CONSOLE_USE_FONT_14x30 1
#define CONSOLE_USE_FONT_16x32 1
#define CONSOLE_USE_FONT_16x37 1
#define CONSOLE_USE_FONT_25x57 1
#endif

typedef struct {
    unsigned char_width;
    unsigned char_height;
    const char * font_name;
    unsigned char * font_bitmap;
} font_t;

extern const font_t console_fonts[];

#endif /* FONT_H_ */
