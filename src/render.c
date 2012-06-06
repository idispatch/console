#include <SDL.h>
#include "console.h"

static Uint32 g_cursor_color;
static Uint32 g_palette[CONSOLE_NUM_PALETTE_ENTRIES];
static SDL_Surface * g_fontSurface = NULL;

static void render_init_font_surface(SDL_Surface * dst, console_t console) {
    unsigned charWidth = console_get_char_width(console);
    unsigned charHeight = console_get_char_height(console);
    if(g_fontSurface) {
        SDL_FreeSurface(g_fontSurface);
        g_fontSurface = NULL;
    }
    g_fontSurface = SDL_CreateRGBSurface(0,//SDL_HWSURFACE,
                                        charWidth * 16,
                                        charHeight * 16,
                                        dst->format->BitsPerPixel,
                                        0xFF000000,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF);
    if(!g_fontSurface)
        return;
    if (SDL_LockSurface(g_fontSurface) != 0)
        return;
    unsigned bpp = g_fontSurface->format->BytesPerPixel;
    unsigned c;
    Uint8 * pixels = (Uint8 *)g_fontSurface->pixels;
    Uint16 pitch = g_fontSurface->pitch;
    for(c = 0; c < 256; c++) {
        unsigned char const * char_bitmap = console_get_char_bitmap(console, (unsigned char)c);
        unsigned x, y;
        Uint8 * char_row = pixels + (c / 16) * pitch * charHeight + (c % 16) * charWidth * bpp;
        for (y = 0; y < charHeight; ++y) {
            unsigned char mask = 0x80;
            unsigned char b = *(char_bitmap + y);
            Uint8 * char_column = char_row;
            for (x = 0; x < charWidth; ++x) {
                if(mask & b) {
                    *char_column++ = 255;
                    *char_column++ = 255;
                    *char_column++ = 255;
                    *char_column++ = 255;
                } else {
                    *char_column++ = 255;
                    *char_column++ = 0;
                    *char_column++ = 0;
                    *char_column++ = 0;
                }
                mask >>= 1;
            }
            char_row += pitch;
        }
    }
    SDL_UnlockSurface(g_fontSurface);
}

void render_init(SDL_Surface * dst, console_t console){
    int i;
    console_rgb_t rgb[CONSOLE_NUM_PALETTE_ENTRIES];
    console_get_palette(console, &rgb[0]);
    for(i=0; i<CONSOLE_NUM_PALETTE_ENTRIES; i++) {
        g_palette[i] = SDL_MapRGB(dst->format, rgb[i].r, rgb[i].g, rgb[i].b);
    }
    g_cursor_color = SDL_MapRGB(dst->format, 255, 255, 255);
    render_init_font_surface(dst, console);
}

void render_done() {
    if(g_fontSurface) {
        SDL_FreeSurface(g_fontSurface);
        g_fontSurface = NULL;
    }
    memset(g_palette, 0, sizeof(g_palette));
}

void render_cursor(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y) {
    SDL_Rect d;
    d.x = x;
    d.y = y + console_get_char_height(console) - 2;
    d.w = console_get_char_width(console);
    d.h = 2;
    SDL_FillRect(dst, &d, g_cursor_color);
}

void render_char(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char c) {
    SDL_Rect srect;
    SDL_Rect drect;
    unsigned w = srect.w = drect.w = console_get_char_width(console);
    srect.h = drect.h = console_get_char_height(console);
    srect.x = ((unsigned char)c % 16) * w;
    srect.y = ((unsigned char)c / 16) * srect.h;
    drect.x = x;
    drect.y = y;
    SDL_BlitSurface(g_fontSurface, &srect, dst, &drect);
}

void render_string(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char * str) {
    SDL_Rect srect;
    SDL_Rect drect;
    unsigned w = srect.w = drect.w = console_get_char_width(console);
    srect.h = drect.h = console_get_char_height(console);
    drect.y = y;
    int n;
    for(n = 0; *str; ++str, ++n) {
        unsigned char c = (unsigned char)*str;
        srect.x = (c % 16) * w;
        srect.y = (c / 16) * w;
        drect.x = n * w;
        SDL_BlitSurface(g_fontSurface, &srect, dst, &drect);
    }
}

Uint32 render_get_background_color(console_t console) {
    return g_palette[console_get_background_color(console)];
}

Uint32 render_get_foreground_color(console_t console) {
    return g_palette[console_get_foreground_color(console)];
}

#ifdef _DEBUG
void render_font_surface(console_t console, SDL_Surface * dst) {
    SDL_Rect srect;
    SDL_Rect drect;
    srect.x = 0;
    srect.y = 0;
    srect.w = console_get_char_width(console) * 16;
    srect.h = console_get_char_height(console) * 16;
    drect.x = srect.x;
    drect.y = srect.y;
    drect.w = srect.w;
    drect.h = srect.h;
    SDL_BlitSurface(g_fontSurface, &srect, dst, &drect);
}
#endif
