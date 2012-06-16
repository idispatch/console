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

typedef enum {
#ifdef CONSOLE_USE_FONT_4x6
    FONT_4x6,
#endif
#ifdef CONSOLE_USE_FONT_4x7
    FONT_4x7,
#endif
#ifdef CONSOLE_USE_FONT_5x8
    FONT_5x8,
#endif
#ifdef CONSOLE_USE_FONT_5x12
    FONT_5x12,
#endif
#ifdef CONSOLE_USE_FONT_6x8
    FONT_6x8,
#endif
#ifdef CONSOLE_USE_FONT_7x9
    FONT_7x9,
#endif
#ifdef CONSOLE_USE_FONT_8x8
    FONT_8x8,
#endif
#ifdef CONSOLE_USE_FONT_8x10
    FONT_8x10,
#endif
#ifdef CONSOLE_USE_FONT_8x16
    FONT_8x16,
#endif
#ifdef CONSOLE_USE_FONT_9x8
    FONT_9x8,
#endif
#ifdef CONSOLE_USE_FONT_9x16
    FONT_9x16,
#endif
#ifdef CONSOLE_USE_FONT_10x20
    FONT_10x20,
#endif
#ifdef CONSOLE_USE_FONT_12x16
    FONT_12x16,
#endif
#ifdef CONSOLE_USE_FONT_12x23
    FONT_12x23,
#endif
#ifdef CONSOLE_USE_FONT_12x24
    FONT_12x24,
#endif
#ifdef CONSOLE_USE_FONT_12x27
    FONT_12x27,
#endif
#ifdef CONSOLE_USE_FONT_14x30
    FONT_14x30,
#endif
#ifdef CONSOLE_USE_FONT_16x32
    FONT_16x32,
#endif
#ifdef CONSOLE_USE_FONT_16x37
    FONT_16x37,
#endif
#ifdef CONSOLE_USE_FONT_25x57
    FONT_25x57,
#endif
} font_id_t;

#endif /* FONT_H_ */
