#include "console.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

struct console {
    unsigned height;
    unsigned width;

    unsigned char_height;
    unsigned char_width;

    unsigned x;
    unsigned y;
    struct console_cell_attribute attribute;

    struct console_cell * buffer;
};

console_t console_alloc(unsigned width, unsigned height) {
    console_t console = (console_t)malloc(sizeof(struct console));
    console->char_width = 8;
    console->char_height = 8;
    console->width = width / 8;
    console->height = height / 8;
    console->buffer = malloc(width * height * sizeof(struct console_cell));
    console_clear(console);
    return console;
}

void console_free(console_t console) {
    if(console) {
        free(console->buffer);
        free(console);
    }
}

void console_clear(console_t console) {
    console->x = 0;
    console->y = 0;
    console->attribute.foreground.r = 255;
    console->attribute.foreground.g = 255;
    console->attribute.foreground.b = 255;
    console->attribute.background.r = 0;
    console->attribute.background.g = 0;
    console->attribute.background.b = 0;
}

void console_print_char(console_t console, int c) {
    struct console_cell * dst = &console->buffer[console->y * console->width + console->x];
    dst->character = (char)(c & 0xff);
    memcpy(&dst->attribute, &console->attribute, sizeof(struct console_cell_attribute));
    if(++(console->x) >= console->width) {
        console->x = 0;
        if(++(console->y) >= console->height) {
            console->y = console->height-1;
            console_scroll_lines(console, 1);
        }
    }
}

void console_print_string(console_t console, const char * str) {
    if(!str || !*str)
        return;
    do {
        if(*str == '\n') { /* New line */
            console->x = 0;
            console->y += 1;
            if(console->y >= console->height) {
                console->y = console->height - 1;
                console_scroll_lines(console, 1);
            }
            str++;
            continue;
        }
        if(*str == '\t') { /* Tabulate */
            int i;
            for(i = 0; i < 4; i++) {
                console_print_char(console, ' ');
            }
            str++;
            continue;
        }
        if(*str == '\f') { /* form feed */
            console_scroll_lines(console, console->height);
            str++;
            continue;
        }
        console_print_char(console, *str++);
    } while(*str);
}

void console_set_foreground_attr_rgb(console_t console, unsigned char r, unsigned char g, unsigned char b) {
    console->attribute.foreground.r = r;
    console->attribute.foreground.g = g;
    console->attribute.foreground.b = b;
}

void console_set_background_attr_rgb(console_t console, unsigned char r, unsigned char g, unsigned char b) {
    console->attribute.background.r = r;
    console->attribute.background.g = g;
    console->attribute.background.b = b;
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
    unsigned w = console->width;
    unsigned h = console->height;
    if(y < console->height) {
        int i;
        struct console_cell * dst = console->buffer;
        struct console_cell * src = console->buffer + y * w;
        int n = console->height - y;
        size_t row_size_bytes = sizeof(struct console_cell) * w;
        for (i = 0; i < n; ++i) {
            memcpy(dst, src, row_size_bytes);
            dst += w;
            src += w;
        }
    }
    for(y = max(0, console->height - y); y < h; ++y) {
        struct console_cell * dst = &console->buffer[y * w];
        unsigned x;
        for(x = 0; x < w; ++x, ++dst) {
            dst->character = 0;
            memcpy(&dst->attribute, &console->attribute, sizeof(struct console_cell_attribute));
        }
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
    return console->buffer[y * console->width + x].character;
}

void console_get_string_at(console_t console, unsigned x, unsigned y, char * buffer, size_t num_bytes) {
    if(num_bytes < 1)
        return;
    if(x >= console->width || y >= console->height) {
        buffer[0] = 0;
    }
    y = y * console->width;
    unsigned w = console->width;
    struct console_cell * src = &console->buffer[y + x];
    num_bytes--; /* reserve for terminating zero */
    for(;num_bytes > 0 && x < w && src->character; --num_bytes, ++src, ++x) {
        *buffer++ = src->character;
    }
    *buffer = 0;
}
