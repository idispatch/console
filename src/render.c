#include <SDL.h>
#include "render.h"
#include "console.h"

static Uint32 g_cursor_color;
static Uint32 g_palette[CONSOLE_NUM_PALETTE_ENTRIES];
#ifdef USE_FONT_SURFACE
static SDL_Surface * g_fontSurface = NULL;
#endif
static unsigned char * g_fontBitmap = NULL;

static Uint32 render_get_background_color(console_t console) {
    return g_palette[console_get_background_color(console)];
}

static void render_init_font_bitmap(console_t console) {
    if(g_fontBitmap) {
        free(g_fontBitmap);
        g_fontBitmap = NULL;
    }
    unsigned c;
    unsigned charWidth = console_get_char_width(console);
    unsigned charHeight = console_get_char_height(console);
    size_t bytes = 256 * charWidth * charHeight * sizeof(unsigned char);
    g_fontBitmap = malloc(bytes);
    unsigned char * char_data_ptr = g_fontBitmap;
    for(c = 0; c < 256; c++) {
        unsigned char const * char_bitmap = console_get_char_bitmap(console, (unsigned char)c);
        unsigned y;
        for (y = 0; y < charHeight; ++y) {
            unsigned char mask = 0x80;
            unsigned char b = *char_bitmap;
            unsigned x;
            for (x = 0; x < charWidth; ++x, mask >>= 1)
                *char_data_ptr++ = (mask & b) ? 255 : 0;
            char_bitmap += 1;
        }
    }
}

#ifdef USE_FONT_SURFACE
static void render_init_font_surface(console_t console, SDL_Surface * dst) {
    if(g_fontSurface) {
        SDL_FreeSurface(g_fontSurface);
        g_fontSurface = NULL;
    }
    unsigned charWidth = console_get_char_width(console);
    unsigned charHeight = console_get_char_height(console);
    g_fontSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                        charWidth * 16,
                                        charHeight * 16,
                                        dst->format->BitsPerPixel,
                                        dst->format->Rmask,
                                        dst->format->Gmask,
                                        dst->format->Bmask,
                                        dst->format->Amask);
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
                switch(bpp) {
                case 3:
                    if(mask & b) {
                        *char_column++ = 255;
                        *char_column++ = 255;
                        *char_column++ = 255;
                    } else {
                        *char_column++ = 0;
                        *char_column++ = 0;
                        *char_column++ = 0;
                    }
                    break;
                case 4:
                    if(mask & b) {
                        *char_column++ = 255;
                        *char_column++ = 255;
                        *char_column++ = 255;
                        *char_column++ = 255;
                    } else {
                        *char_column++ = 0;
                        *char_column++ = 0;
                        *char_column++ = 0;
                        *char_column++ = 0;
                    }
                    break;
                }
                mask >>= 1;
            }
            char_row += pitch;
        }
    }
    SDL_UnlockSurface(g_fontSurface);
}
#endif

static void render_init_font(console_t console, SDL_Surface * dst) {
#ifdef USE_FONT_SURFACE
    render_init_font_surface(console, dst);
#else
    render_init_font_bitmap(console);
#endif
}

void render_init(console_t console, SDL_Surface * dst){
    int i;
    console_rgb_t rgb[CONSOLE_NUM_PALETTE_ENTRIES];
    console_get_palette(console, &rgb[0]);
    for(i=0; i<CONSOLE_NUM_PALETTE_ENTRIES; i++) {
        g_palette[i] = SDL_MapRGB(dst->format, rgb[i].r, rgb[i].g, rgb[i].b);
    }
    g_cursor_color = SDL_MapRGB(dst->format, 255, 255, 255);
    render_init_font(console, dst);
}

void render_done() {
#ifdef USE_FONT_SURFACE
    if(g_fontSurface) {
        SDL_FreeSurface(g_fontSurface);
        g_fontSurface = NULL;
    }
#endif
    if(g_fontBitmap) {
        free(g_fontBitmap);
        g_fontBitmap = NULL;
    }
    memset(g_palette, 0, sizeof(g_palette));
}

static void render_cursor(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y) {
    unsigned char_width = console_get_char_width(console);
    unsigned char_height = console_get_char_height(console);
    SDL_Rect d;
    d.x = x * char_width;
    d.y = (y + 1) * char_height - 3;
    d.w = char_width;
    d.h = 2;
    SDL_FillRect(dst, &d, g_cursor_color);
}

static void render_char_bitmap(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char c, unsigned char a) {
    Uint32 foreColor = g_palette[a & 0xf];
    Uint32 backColor = g_palette[a >> 4];
    if (SDL_LockSurface(dst) != 0)
        return;
    unsigned bpp = dst->format->BytesPerPixel;
    unsigned cx;
    unsigned cy;
    unsigned charWidth = console_get_char_width(console);
    unsigned charHeight = console_get_char_height(console);
    Uint8 * pixels = (Uint8 *)dst->pixels + y * charHeight * dst->pitch + x * charWidth * bpp;
    Uint16 pitch = dst->pitch;
    unsigned char * bitmap_data = g_fontBitmap + c * charWidth * charHeight * sizeof(unsigned char);
    for (cy = 0; cy < charHeight; ++cy) {
        Uint32 * dst_ptr = (Uint32*)pixels;
        for (cx = 0; cx < charWidth; ++cx) {
            *dst_ptr++ = (*bitmap_data++) ? foreColor : backColor;
        }
        pixels += pitch;
    }
    SDL_UnlockSurface(dst);
}

#ifdef USE_FONT_SURFACE
static void render_char_surface(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char c, unsigned char a) {
    SDL_Rect srect;
    SDL_Rect drect;
    unsigned w = srect.w = drect.w = console_get_char_width(console);
    unsigned h = srect.h = drect.h = console_get_char_height(console);
    srect.x = ((unsigned char)c % 16) * w;
    srect.y = ((unsigned char)c / 16) * h;
    drect.x = x * w;
    drect.y = y * h;
    SDL_BlitSurface(g_fontSurface, &srect, dst, &drect);
}
#endif

static void render_char(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char c, unsigned char a) {
#ifdef USE_FONT_SURFACE
    render_char_surface(console, dst, x, y, c, a);
#else
    render_char_bitmap(console, dst, x, y, c, a);
#endif
}
/*
static void render_string(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char * str) {
    SDL_Rect srect;
    SDL_Rect drect;
    unsigned char_width = srect.w = drect.w = console_get_char_width(console);
    unsigned char_height = srect.h = drect.h = console_get_char_height(console);
    drect.y = y * char_height;
    int n;
    for(n = 0; *str; ++str, ++n) {
        unsigned char c = (unsigned char)*str;
        srect.x = (c % 16) * char_width;
        srect.y = (c / 16) * char_height;
        drect.x = (n + x) * char_width;
        SDL_BlitSurface(g_fontSurface, &srect, dst, &drect);
    }
}
*/

#ifdef _DEBUG
#if 0
static void render_font_surface(console_t console, SDL_Surface * dst) {
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
#endif

void console_render_callback(console_t console, console_update_t * u, void * data) {
    SDL_Surface * screen = (SDL_Surface *)data;
    switch(u->type) {
    case CONSOLE_UPDATE_CHAR:
        render_char(console,
                    screen,
                    u->data.u_char.x,
                    u->data.u_char.y,
                    u->data.u_char.c,
                    u->data.u_char.a);
        fprintf(stdout, "CONSOLE_UPDATE_CHAR: [%d,%d]=%c\n", u->data.u_char.x, u->data.u_char.y, u->data.u_char.c);
        fflush(stdout);
        break;
    case CONSOLE_UPDATE_ROWS:
        break;
    case CONSOLE_UPDATE_SCROLL: {
            SDL_Rect src;
            SDL_Rect dst;
            unsigned console_width = console_get_width(console);
            unsigned console_height = console_get_height(console);
            unsigned char_width = console_get_char_width(console);
            unsigned char_height = console_get_char_height(console);
            src.x = dst.x = 0;
            src.w = dst.w = char_width * console_width;
            src.h = dst.h = u->data.u_scroll.n * char_height;
            src.y = u->data.u_scroll.y2 * char_height;
            dst.y = u->data.u_scroll.y1 * char_height;
            SDL_BlitSurface(screen, &src, screen, &dst);
            dst.y = (console_height * char_height) - (console_get_height(console) - u->data.u_scroll.n) * char_height;
            dst.h = (u->data.u_scroll.y2 - u->data.u_scroll.y1) * char_height;
            SDL_FillRect(screen, &dst, render_get_background_color(console));
            SDL_UpdateRect(screen, 0, 0, 0, 0);
        }
        break;
    case CONSOLE_UPDATE_CURSOR_VISIBILITY:
        if(u->data.u_cursor.cursor_visible) {
            render_cursor(console,
                          screen,
                          console_get_cursor_x(console),
                          console_get_cursor_y(console));
        } else {
            render_char(console,
                        screen,
                        console_get_cursor_x(console),
                        console_get_cursor_y(console),
                        console_get_character_at(console,
                                                 console_get_cursor_x(console),
                                                 console_get_cursor_y(console)),
                        console_get_attribute_at(console,
                                                 console_get_cursor_x(console),
                                                 console_get_cursor_y(console)));
        }
        break;
    case CONSOLE_UPDATE_CURSOR_POSITION:
        fprintf(stdout, "CONSOLE_UPDATE_CURSOR_POSITION: [%d,%d->%d,%d]=%d\n",
                u->data.u_cursor.x, u->data.u_cursor.y,
                console_get_cursor_x(console),
                console_get_cursor_y(console),
                (int)u->data.u_cursor.cursor_visible);
        fflush(stdout);
        render_char(console,
                    screen,
                    u->data.u_cursor.x,
                    u->data.u_cursor.y,
                    console_get_character_at(console,
                                             u->data.u_cursor.x,
                                             u->data.u_cursor.y),
                    console_get_attribute_at(console,
                                             u->data.u_cursor.x,
                                             u->data.u_cursor.y));
        if(u->data.u_cursor.cursor_visible) {
            render_cursor(console,
                          screen,
                          console_get_cursor_x(console),
                          console_get_cursor_y(console));
        }
        break;
    }
}
