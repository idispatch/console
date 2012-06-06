#include "console.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

extern unsigned char console_font_8x8[];

struct console {
    unsigned height;
    unsigned width;

    unsigned char_height;
    unsigned char_width;

    unsigned x;
    unsigned y;
    unsigned char attribute;

    char * character_buffer;
    unsigned char * attribute_buffer;

    console_rgb_t palette[16];
    unsigned char * font_bitmap;
    console_callback_t callback;
};

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

static void console_callback(console_t console, console_update_t * p) {
}

console_t console_alloc(unsigned width, unsigned height) {
    console_t console = (console_t)malloc(sizeof(struct console));
    console_set_callback(console, 0);
    console_set_font(console, FONT_8x8_SYSTEM);
    console_set_palette(console, &g_palette[0]);
    console->width = width / console->char_width;
    console->height = height / console->char_height;
    size_t num_cells = console->width * console->height;
    console->character_buffer = malloc(num_cells * sizeof(char));
    console->attribute_buffer = malloc(num_cells * sizeof(unsigned char));
    console_clear(console);
    return console;
}

void console_free(console_t console) {
    if(console) {
        free(console->character_buffer);
        free(console->attribute_buffer);
        free(console);
    }
}

void console_set_callback(console_t console, console_callback_t callback) {
    console->callback = callback == 0 ? console_callback : callback;
}

void console_set_palette(console_t console, console_rgb_t const * palette) {
    memcpy(console->palette, palette, sizeof(console_rgb_t) * 16);
    console_update_t u;
    u.type = CONSOLE_UPDATE_PALETTE;
    u.data.u_palette.palette = console->palette;
    console->callback(console, &u);
}

void console_get_palette(console_t console, console_rgb_t * palette) {
    memcpy(palette, console->palette, sizeof(console_rgb_t) * 16);
}

void console_set_font(console_t console, unsigned font) {
    unsigned char * old_font = console->font_bitmap;
    switch(font) {
    case FONT_8x8_SYSTEM:
        console->font_bitmap = console_font_8x8;
        console->char_width = 8;
        console->char_height = 8;
        break;
    default:
        console->font_bitmap = console_font_8x8;
        console->char_width = 8;
        console->char_height = 8;
        break;
    }
    console_update_t u;
    u.data.u_font.font_bitmap = console->font_bitmap;
    u.type = CONSOLE_UPDATE_FONT;
    console->callback(console, &u);
}

void console_clear(console_t console) {
    console->x = 0;
    console->y = 0;
    console->attribute = 0xf;
    unsigned n = console->height * console->width;
    memset(console->character_buffer, 0, n);
    memset(console->attribute_buffer, console->attribute, n);
    console_update_t u;
    u.type = CONSOLE_UPDATE_ROWS;
    u.data.u_rows.x1 = 0;
    u.data.u_rows.y1 = 0;
    u.data.u_rows.x2 = console->width;
    u.data.u_rows.y2 = console->height;
    console->callback(console, &u);
}

void console_print_char(console_t console, char c) {
    if(c == '\n') { /* New line */
        console->x = 0;
        console->y += 1;
        if(console->y >= console->height) {
            console->y = console->height - 1;
            console_scroll_lines(console, 1);
        }
        return;
    }
    if(c == '\t') { /* Tabulate */
        int i;
        for(i = 0; i < 4; i++) {
            console_print_char(console, ' ');
        }
        return;
    }
    if(c == '\f') { /* Form feed */
        console_scroll_lines(console, console->height);
        return;
    }
    console_update_t u;
    u.type = CONSOLE_UPDATE_CHAR;
    u.data.u_char.x = console->x;
    u.data.u_char.y = console->y;
    u.data.u_char.c = c;
    u.data.u_char.a = console->attribute;
    size_t offset = console->y * console->width + console->x;
    console->character_buffer[offset] = c;
    console->attribute_buffer[offset] = console->attribute;
    console->callback(console, &u);
    if(++console->x >= console->width) {
        console->x = 0;
        if(++console->y >= console->height) {
            console->y = console->height-1;
            console_scroll_lines(console, 1);
        }
    }
}

void console_print_string(console_t console, const char * str) {
    if(!str || !*str)
        return;
    console_update_t u;
    u.type = CONSOLE_UPDATE_ROWS;
    u.data.u_rows.x1 = console->x;
    u.data.u_rows.y1 = console->y;
    do {
        console_print_char(console, *str++);
    } while(*str);
    u.data.u_rows.x2 = console->x;
    u.data.u_rows.y2 = console->y;
    console->callback(console, &u);
}

void console_set_attr(console_t console, unsigned char attr) {
    console->attribute = attr;
}

void console_goto_xy(console_t console, unsigned x, unsigned y) {
    if(x >= console->width)
        x = console->width - 1;
    if(y >= console->height)
        y = console->height - 1;
    console->x = x;
    console->y = y;
}

void console_scroll_lines(console_t console, unsigned y) {
    if(y == 0)
        return;
    console_update_t u;
    u.type = CONSOLE_UPDATE_SCROLL;
    u.data.u_scroll.y1 = 0;
    unsigned w = console->width;
    unsigned h = console->height;
    unsigned offset = y * w;
    if(y < console->height) {
        int n = (console->height - y) * w;
        memmove(console->character_buffer, console->character_buffer + offset, n);
        memmove(console->attribute_buffer, console->attribute_buffer + offset, n);
    }
    if(y < h) {
        u.data.u_scroll.y2 = y;
        u.data.u_scroll.n = h - y;
        offset = (h - y) * w;
        y = y * w;
        memset(console->character_buffer + offset, 0, y);
        memset(console->attribute_buffer + offset, console->attribute, y);
        console->callback(console, &u);
    } else {
        u.data.u_scroll.y2 = h;
        u.data.u_scroll.n = h;
        y = h * w;
        memset(console->character_buffer, 0, y);
        memset(console->attribute_buffer, console->attribute, y);
    }
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

unsigned console_get_x(console_t console) {
    return console->x;
}

unsigned console_get_y(console_t console) {
    return console->y;
}

int console_get_char_at(console_t console, unsigned x, unsigned y) {
    if(x >= console->width || y >= console->height)
        return 0;
    return console->character_buffer[y * console->width + x];
}

unsigned char console_get_attr_at(console_t console, unsigned x, unsigned y) {
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

void console_get_string_at(console_t console, unsigned x, unsigned y, char * buffer, size_t num_bytes) {
    if(num_bytes < 1)
        return;
    if(x >= console->width || y >= console->height) {
        buffer[0] = 0;
    }
    y = y * console->width;
    unsigned w = console->width;
    const char * src = &console->character_buffer[y + x];
    num_bytes--; /* reserve for terminating zero */
    for(;num_bytes > 0 && x < w && *src; --num_bytes, ++x) {
        *buffer++ = *src++;
    }
    *buffer = 0;
}

void console_get_color_at(console_t console, unsigned x, unsigned y, console_rgb_t * foreground, console_rgb_t * background) {
    unsigned char attr = console_get_attr_at(console, x, y);
    console_rgb_t const * f = &console->palette[attr & 0x0f];
    console_rgb_t const * b = &console->palette[(attr & 0xf0) >> 4];
    foreground->r = f->r;
    foreground->g = f->g;
    foreground->b = f->b;
    background->r = b->r;
    background->g = b->g;
    background->b = b->b;
}

unsigned char * console_get_char_bitmap(console_t console, unsigned char c) {
    unsigned bytes_per_row = console->char_width / 8;
    unsigned bytes_per_char = bytes_per_row * console->char_height;
    unsigned offset = c * bytes_per_char;
    return &console->font_bitmap[offset];
}
