#include "console.h"
#include "font.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

struct console {
    unsigned height;
    unsigned width;

    unsigned view_height;
    unsigned view_width;

    unsigned char_height;
    unsigned char_width;

    unsigned cursor_x;
    unsigned cursor_y;
    unsigned saved_cursor_x;
    unsigned saved_cursor_y;
    unsigned char attribute;

    char * character_buffer;
    unsigned char * attribute_buffer;

    unsigned tab_width;
    unsigned char cursor_state;
    unsigned cursor_blink_rate;
    console_rgb_t palette[16];
    unsigned char * font_bitmap;
    console_callback_t callback;
    void * callback_data;
};

#define CURSOR_VISIBLE 1
#define CURSOR_SHOWN 2

/* http://en.wikipedia.org/wiki/ANSI_escape_code*/
static console_rgb_t g_palette[] = {
    /* normal */
    {0,   0,   0}, /* black */
    {128, 0,   0}, /* red */
    {0,   128, 0}, /* green */
    {128, 128, 0}, /* yellow */
    {0,   0,   128}, /* blue */
    {128, 0,   128}, /* magenta */
    {0,   128, 128}, /* cyan */
    {192, 192, 192}, /* gray */
    /* bright */
    {128, 128, 128}, /* dark gray */
    {255, 0,   0}, /* red */
    {0,   255, 0}, /* green */
    {255, 255, 0}, /* yellow */
    {0,   0,   255}, /* blue */
    {255, 0,   255}, /* magenta */
    {0,   255, 255}, /* cyan */
    {255, 255, 255}, /* white */
};

static void console_callback(console_t console, console_update_t * p, void * data) {
}

console_t console_alloc(unsigned width, unsigned height) {
    console_t console = (console_t)calloc(1, sizeof(struct console));
    console->view_width = width;
    console->view_height = height;
    console_set_tab_width(console, 4);
    console_set_callback(console, NULL, NULL);
    console_set_palette(console, &g_palette[0]);
    console_set_font(console, FONT_8x8_SYSTEM);
    console_set_cursor_blink_rate(console, 200);
    console_show_cursor(console);
    console_clear(console);
    return console;
}

void console_free(console_t console) {
    if(console) {
        console->callback_data = NULL;
        free(console->character_buffer);
        free(console->attribute_buffer);
        free(console);
    }
}

void console_save_cursor_position(console_t console) {
    console->saved_cursor_x = console->cursor_x;
    console->saved_cursor_y = console->cursor_y;
}

void console_restore_cursor_position(console_t console) {
    console->cursor_x = console->saved_cursor_x;
    console->cursor_y = console->saved_cursor_y;
}

void console_set_tab_width(console_t console, unsigned width) {
    console->tab_width = 4;
}

unsigned console_get_tab_width(console_t console) {
    return console->tab_width;
}

bool console_cursor_is_visible(console_t console) {
    return console->cursor_state & CURSOR_VISIBLE;
}

bool console_cursor_is_shown(console_t console) {
    return (console->cursor_state & (CURSOR_VISIBLE | CURSOR_SHOWN)) == (CURSOR_VISIBLE | CURSOR_SHOWN);
}

void console_set_cursor_blink_rate(console_t console, unsigned rate) {
    console->cursor_blink_rate = rate;
}

unsigned console_get_cursor_blink_rate(console_t console) {
    return console->cursor_blink_rate;
}

void console_show_cursor(console_t console) {
    if(console->cursor_state & CURSOR_VISIBLE)
        return;
    console->cursor_state |= CURSOR_VISIBLE;
    console_update_t u;
    u.type = CONSOLE_UPDATE_CURSOR_VISIBILITY;
    u.data.u_cursor.cursor_visible = true;
    u.data.u_cursor.x = console->cursor_x;
    u.data.u_cursor.y = console->cursor_y;
    console->callback(console, &u, console->callback_data);
}

void console_hide_cursor(console_t console) {
    if(!(console->cursor_state & CURSOR_VISIBLE))
        return;
    console->cursor_state &= ~(CURSOR_VISIBLE | CURSOR_SHOWN);
    console_update_t u;
    u.type = CONSOLE_UPDATE_CURSOR_VISIBILITY;
    u.data.u_cursor.cursor_visible = false;
    u.data.u_cursor.x = console->cursor_x;
    u.data.u_cursor.y = console->cursor_y;
    console->callback(console, &u, console->callback_data);
}

void console_blink_cursor(console_t console) {
    if(!console_cursor_is_visible(console))
        return; /* cursor is not visible */
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t milliseconds = (uint64_t)t.tv_sec * 1000 + (uint64_t)t.tv_nsec / 1000000;
    bool shown = (milliseconds % (console->cursor_blink_rate * 2)) < console->cursor_blink_rate ? true : false;
    bool was_shown = (((console->cursor_state & CURSOR_SHOWN) >> 1) ? true : false);
    if(shown != was_shown) {
        console->cursor_state ^= CURSOR_SHOWN;
        console_update_t u;
        u.type = CONSOLE_UPDATE_CURSOR_VISIBILITY;
        u.data.u_cursor.cursor_visible = console_cursor_is_shown(console);
        u.data.u_cursor.x = console->cursor_x;
        u.data.u_cursor.y = console->cursor_y;
        console->callback(console, &u, console->callback_data);
    }
}

void console_set_callback(console_t console, console_callback_t callback, void * data) {
    console->callback = callback == 0 ? console_callback : callback;
    console->callback_data = data;
}

void console_set_palette(console_t console, console_rgb_t const * palette) {
    memcpy(console->palette, palette, sizeof(console_rgb_t) * 16);
    console_update_t u;
    u.type = CONSOLE_UPDATE_PALETTE;
    u.data.u_palette.palette = console->palette;
    console->callback(console, &u, console->callback_data);
}

void console_get_palette(console_t console, console_rgb_t * palette) {
    memcpy(palette, console->palette, sizeof(console_rgb_t) * 16);
}

void console_set_font(console_t console, font_id_t font) {
    unsigned char * old_font = console->font_bitmap;
    unsigned char * new_font = console_fonts[font].font_bitmap;
    if(old_font == new_font)
        return;

    console->char_height = console_fonts[font].char_height;
    console->char_width = console_fonts[font].char_width;
    console->font_bitmap = new_font;

    console->width = console->view_width / console->char_width;
    console->height = console->view_height / console->char_height;

    size_t num_cells = console->width * console->height;
    console->character_buffer = realloc(console->character_buffer, num_cells * sizeof(char));
    console->attribute_buffer = realloc(console->attribute_buffer, num_cells * sizeof(unsigned char));

    console_update_t u;
    u.type = CONSOLE_UPDATE_FONT;
    u.data.u_font.char_width = console->char_width;
    u.data.u_font.char_height = console->char_height;
    u.data.u_font.font_bitmap = console->font_bitmap;
    console->callback(console, &u, console->callback_data);
}

static void console_update_rows(console_t console, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
    console_update_t u;
    u.type = CONSOLE_UPDATE_ROWS;
    u.data.u_rows.x1 = x1;
    u.data.u_rows.y1 = y1;
    u.data.u_rows.x2 = x2;
    u.data.u_rows.y2 = y2;
    console->callback(console, &u, console->callback_data);
}

void console_clear(console_t console) {
    console_cursor_goto_xy(console, 0, 0);
    console->attribute = 0xf;
    size_t bytes = console->height * console->width;
    memset(console->character_buffer, 0, bytes);
    memset(console->attribute_buffer, console->attribute, bytes);
    console_update_rows(console, 0, 0, console->width, console->height);
}

static void console_cursor_advance(console_t console) {
    unsigned x = console->cursor_x;
    unsigned y = console->cursor_y;
    if(++x >= console->width) {
        x = 0;
        if(++y >= console->height) {
            y = console->height-1;
            console_scroll_lines(console, 1);
        }
    }
    console_cursor_goto_xy(console, x, y);
}

void console_print_char(console_t console, char c) {
    if(c == '\n') {
        unsigned x = 0;
        unsigned y = console->cursor_y;
        if(++y >= console->height) {
            y = console->height - 1;
            console_scroll_lines(console, 1);
        }
        console_cursor_goto_xy(console, x, y);
        return;
    }
    if(c == '\t') {
        unsigned i;
        for(i = 0; i < console->tab_width; i++) {
            console_print_char(console, ' ');
        }
        return;
    }
    /*
    if(c == '\f') {
        console_scroll_lines(console, console->height);
        return;
    }*/

    size_t offset = console->cursor_y * console->width + console->cursor_x;
    console->character_buffer[offset] = c;
    console->attribute_buffer[offset] = console->attribute;

    console_update_t u;
    u.type = CONSOLE_UPDATE_CHAR;
    u.data.u_char.x = console->cursor_x;
    u.data.u_char.y = console->cursor_y;
    u.data.u_char.c = c;
    u.data.u_char.a = console->attribute;
    console->callback(console, &u, console->callback_data);

    console_cursor_advance(console);
}

void console_print_string(console_t console, const char * str) {
    if(!str || !*str)
        return;
    unsigned x1 = console->cursor_x;
    unsigned y1 = console->cursor_y;
    do {
        console_print_char(console, *str++);
    } while(*str);
    console_update_rows(console, x1, y1, console->cursor_x, console->cursor_y);
}

void console_set_attribute(console_t console, unsigned char attr) {
    console->attribute = attr;
}

void console_cursor_goto_xy(console_t console, unsigned x, unsigned y) {
    if(x >= console->width)
        x = console->width - 1;
    if(y >= console->height)
        y = console->height - 1;

    if(x==console->cursor_x && y==console->cursor_y)
        return;

    console_update_t u;
    u.type = CONSOLE_UPDATE_CURSOR_POSITION;
    u.data.u_cursor.cursor_visible = true;//console_cursor_is_shown(console);
    u.data.u_cursor.x = console->cursor_x;
    u.data.u_cursor.y = console->cursor_y;

    console->cursor_x = x;
    console->cursor_y = y;

    console->callback(console, &u, console->callback_data);
}

void console_scroll_lines(console_t console, unsigned n) {
    if(n == 0)
        return;
    unsigned w = console->width;
    unsigned h = console->height;

    console_update_t u;
    u.type = CONSOLE_UPDATE_CURSOR_VISIBILITY;
    u.data.u_cursor.cursor_visible = false;
    u.data.u_cursor.x = console->cursor_x;
    u.data.u_cursor.y = console->cursor_y;
    console->callback(console, &u, console->callback_data);

    u.type = CONSOLE_UPDATE_SCROLL;
    u.data.u_scroll.y1 = 0;
    u.data.u_scroll.y2 = min(h, n);
    u.data.u_scroll.n = h - min(h, n);
    console->callback(console, &u, console->callback_data);

    unsigned offset = n * w;
    if(n < console->height) {
        int bytes = (console->height - n) * w;
        memmove(console->character_buffer, console->character_buffer + offset, bytes);
        memmove(console->attribute_buffer, console->attribute_buffer + offset, bytes);
    }
    if(n < h) {
        offset = (h - n) * w;
        n = n * w;
        memset(console->character_buffer + offset, 0, n);
        memset(console->attribute_buffer + offset, console->attribute, n);
    } else {
        n = h * w;
        memset(console->character_buffer, 0, n);
        memset(console->attribute_buffer, console->attribute, n);
    }

    u.type = CONSOLE_UPDATE_CURSOR_VISIBILITY;
    u.data.u_cursor.cursor_visible = console_cursor_is_shown(console);
    u.data.u_cursor.x = console->cursor_x;
    u.data.u_cursor.y = console->cursor_y;
    console->callback(console, &u, console->callback_data);
}

unsigned console_get_width(console_t console) {
    return console->width;
}

unsigned console_get_height(console_t console) {
    return console->height;
}

unsigned console_get_char_width(console_t console) {
    return console->char_width;
}

unsigned console_get_char_height(console_t console) {
    return console->char_height;
}

unsigned console_get_cursor_x(console_t console) {
    return console->cursor_x;
}

unsigned console_get_cursor_y(console_t console) {
    return console->cursor_y;
}

int console_get_character_at(console_t console, unsigned x, unsigned y) {
    if(x >= console->width || y >= console->height)
        return 0;
    return console->character_buffer[y * console->width + x];
}

unsigned char console_get_attribute_at(console_t console, unsigned x, unsigned y) {
    if(x >= console->width || y >= console->height)
        return 0xf;
    return console->attribute_buffer[y * console->width + x];
}

unsigned char console_get_background_color(console_t console) {
    return (console->attribute & 0xf0) >> 4;
}

unsigned char console_get_foreground_color(console_t console) {
    return console->attribute & 0xf;
}

unsigned char * console_get_char_bitmap(console_t console, unsigned char c) {
    unsigned bytes_per_row = console->char_width / 8;
    unsigned bytes_per_char = bytes_per_row * console->char_height;
    unsigned offset = c * bytes_per_char;
    return &console->font_bitmap[offset];
}
