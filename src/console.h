#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <sys/types.h>

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
} console_update_type;

typedef struct {
    console_update_type type;
    union {
        struct {
            unsigned x;
            unsigned y;
            char c;
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
            unsigned char * font_bitmap;
        } u_font;
    } data;
} console_update_t;

struct console;
typedef struct console * console_t;
typedef void (*console_callback_t)(console_t console, console_update_t * p);

#define CONSOLE_NUM_PALETTE_ENTRIES 16
#define FONT_8x8_SYSTEM 0

console_t console_alloc(unsigned width, unsigned height);
void console_free(console_t console);
void console_clear(console_t console);
void console_print_char(console_t console, char c);
void console_print_string(console_t console, const char * str);
void console_goto_xy(console_t console, unsigned x, unsigned y);
void console_scroll_lines(console_t console, unsigned y);
unsigned console_get_width(console_t console);
unsigned console_get_height(console_t console);
unsigned console_get_char_width(console_t console);
unsigned console_get_char_height(console_t console);
unsigned console_get_x(console_t console);
unsigned console_get_y(console_t console);
int console_get_char_at(console_t console, unsigned x, unsigned y);
unsigned char console_get_attr_at(console_t console, unsigned x, unsigned y);
unsigned char console_get_background_color(console_t console);
unsigned char console_get_foreground_color(console_t console);
void console_get_string_at(console_t console, unsigned x, unsigned y, char * buffer, size_t num_bytes);
void console_set_palette(console_t console, console_rgb_t const * palette);
void console_get_palette(console_t console, console_rgb_t * palette);
void console_get_color_at(console_t console, unsigned x, unsigned y, console_rgb_t * foreground, console_rgb_t * background);
void console_set_font(console_t console, unsigned font);
unsigned char * console_get_char_bitmap(console_t console, unsigned char c);
void console_set_callback(console_t console, console_callback_t callback);

#endif /* CONSOLE_H_ */
