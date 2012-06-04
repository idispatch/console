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
    unsigned char attribute;

    char * character_buffer;
    unsigned char * attribute_buffer;

    struct console_rgb palette[16];
};

/* http://en.wikipedia.org/wiki/ANSI_escape_code*/
static struct console_rgb g_palette[] = {
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

console_t console_alloc(unsigned width, unsigned height) {
    console_t console = (console_t)malloc(sizeof(struct console));
    console->char_width = 8;
    console->char_height = 8;
    console->width = width / 8;
    console->height = height / 8;
    size_t num_cells =width * height;
    console->character_buffer = malloc(num_cells * sizeof(char));
    console->attribute_buffer = malloc(num_cells * sizeof(unsigned char));
    console_set_palette(console, &g_palette[0]);
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

void console_set_palette(console_t console, struct console_rgb const * palette) {
    memcpy(console->palette, palette, sizeof(struct console_rgb) * 16);
}

void console_clear(console_t console) {
    console->x = 0;
    console->y = 0;
    console->attribute = 0xf;
}

void console_print_char(console_t console, char c) {
    size_t offset = console->y * console->width + console->x;
    console->character_buffer[offset] = c;
    console->attribute_buffer[offset] = console->attribute;
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
    unsigned w = console->width;
    unsigned h = console->height;
    unsigned offset = y * w;
    if(y < console->height) {
        int n = (console->height - y) * w;
        memmove(console->character_buffer, console->character_buffer + offset, n);
        memmove(console->attribute_buffer, console->attribute_buffer + offset, n);
    }
    y = max(0, console->height - y) * w;
    memset(console->character_buffer + offset, 0, y);
    memset(console->attribute_buffer + offset, console->attribute, y);
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

void console_get_coolor_at(console_t console, unsigned x, unsigned y, struct console_rgb * foreground, struct console_rgb * background) {
    unsigned char attr = console_get_attr_at(console, x, y);
    struct console_rgb const * f = &console->palette[attr & 0x0f];
    struct console_rgb const * b = &console->palette[(attr & 0xf0) >> 4];
    foreground->r = f->r;
    foreground->g = f->g;
    foreground->b = f->b;
    background->r = b->r;
    background->g = b->g;
    background->b = b->b;
}
