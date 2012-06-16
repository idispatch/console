#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <sys/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} console_rgb_t;

typedef enum {
    CONSOLE_UPDATE_CHAR,
    CONSOLE_UPDATE_ROWS,
    CONSOLE_UPDATE_SCROLL,
    CONSOLE_UPDATE_PALETTE,
    CONSOLE_UPDATE_FONT,
    CONSOLE_UPDATE_CURSOR_VISIBILITY,
    CONSOLE_UPDATE_CURSOR_POSITION
} console_update_type;

typedef struct {
    console_update_type type;
    union {
        struct {
            unsigned x;
            unsigned y;
            unsigned char c;
            unsigned char a;
        } u_char;
        struct {
            unsigned x1;
            unsigned y1;
            unsigned x2;
            unsigned y2;
        } u_rows;
        struct {
            unsigned y1;
            unsigned y2;
            unsigned n;
        } u_scroll;
        struct {
            console_rgb_t * palette;
        } u_palette;
        struct {
            unsigned char_width;
            unsigned char_height;
            unsigned char * font_bitmap;
        } u_font;
        struct {
            unsigned x;
            unsigned y;
            bool cursor_visible;
        } u_cursor;
    } data;
} console_update_t;

/* Color indices */
#define CONSOLE_BLACK          0
#define CONSOLE_BLUE           1
#define CONSOLE_GREEN          2
#define CONSOLE_CYAN           3
#define CONSOLE_RED            4
#define CONSOLE_MAGENTA        5
#define CONSOLE_BROWN          6
#define CONSOLE_LIGHT_GRAY     7
#define CONSOLE_DARK_GRAY      8
#define CONSOLE_LIGHT_BLUE     9
#define CONSOLE_LIGHT_GREEN    10
#define CONSOLE_LIGHT_CYAN     11
#define CONSOLE_LIGHT_RED      12
#define CONSOLE_LIGHT_MAGENTA  13
#define CONSOLE_YELLOW         14
#define CONSOLE_WHITE          15

struct console;
typedef struct console * console_t;
typedef void (*console_callback_t)(console_t console, console_update_t * p, void * data);

#define CONSOLE_NUM_PALETTE_ENTRIES 16

#include "font.h"

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

console_t console_alloc(unsigned width, unsigned height);
void console_free(console_t console);
void console_clear(console_t console);
unsigned short * console_get_raw_buffer(console_t console);
void console_set_tab_width(console_t console, unsigned width);
unsigned console_get_tab_width(console_t console);
void console_print_char(console_t console, unsigned char c);
void console_print_string(console_t console, const unsigned char * str);
void console_cursor_goto_xy(console_t console, unsigned x, unsigned y);
void console_save_cursor_position(console_t console);
void console_restore_cursor_position(console_t console);
bool console_cursor_is_visible(console_t console);
bool console_cursor_is_shown(console_t console);
void console_scroll_lines(console_t console, unsigned n);
unsigned console_get_width(console_t console);
unsigned console_get_height(console_t console);
unsigned console_get_char_width(console_t console);
unsigned console_get_char_height(console_t console);
unsigned console_get_cursor_x(console_t console);
unsigned console_get_cursor_y(console_t console);
unsigned char console_get_character_at(console_t console, unsigned x, unsigned y);
unsigned char console_get_character_at_offset(console_t console, unsigned offset);
void console_set_attribute(console_t console, unsigned char attr);
void console_set_character_and_attribute_at(console_t console, unsigned x, unsigned y, unsigned char c, unsigned char attr);
void console_set_character_and_attribute_at_offset(console_t console, unsigned offset, unsigned char c, unsigned char attr);
unsigned char console_get_attribute_at(console_t console, unsigned x, unsigned y);
unsigned char console_get_attribute_at_offset(console_t console, unsigned offset);
unsigned char console_get_background_color(console_t console);
unsigned char console_get_foreground_color(console_t console);
void console_set_palette(console_t console, console_rgb_t const * palette);
void console_get_palette(console_t console, console_rgb_t * palette);
void console_set_font(console_t console, font_id_t font);
unsigned char * console_get_char_bitmap(console_t console, unsigned char c);
void console_set_callback(console_t console, console_callback_t callback, void * data);
void console_set_cursor_blink_rate(console_t console, unsigned rate);
unsigned console_get_cursor_blink_rate(console_t console);
void console_blink_cursor(console_t console);
void console_show_cursor(console_t console);
void console_hide_cursor(console_t console);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H_ */
