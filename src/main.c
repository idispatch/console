#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "console.h"
#include "render.h"

static SDL_Surface *g_screenSurface = NULL;
static console_t g_console = NULL;

static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 600;
static const int SCREEN_BPP = 32;

static void init();
static void shutDown();
static void render();
static void render_callback(console_t console, console_update_t * u);

int main(int argc, char *argv[]) {
    init();
    bool bDone = false;
    while (bDone == false) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                bDone = true;
                break;
            case SDL_KEYDOWN:
                if(isascii(event.key.keysym.unicode) &&
                   isprint(event.key.keysym.unicode & 0xff)) {
                    console_print_char(g_console, (char)(event.key.keysym.unicode & 0xff));
                } else {
                    switch(event.key.keysym.sym) {
                    case SDLK_LEFT: {
                            unsigned x = console_get_cursor_x(g_console);
                            if(x > 0)
                                --x;
                            console_cursor_goto_xy(g_console, x, console_get_cursor_y(g_console));
                        }
                        break;
                    case SDLK_DOWN:{
                            unsigned y = console_get_cursor_y(g_console);
                            if(y < console_get_height(g_console))
                                ++y;
                            console_cursor_goto_xy(g_console, console_get_cursor_x(g_console), y);
                        }
                        break;
                    case SDLK_UP:{
                            unsigned y = console_get_cursor_y(g_console);
                            if(y > 0)
                                --y;
                            console_cursor_goto_xy(g_console, console_get_cursor_x(g_console), y);
                        }
                        break;
                    case SDLK_RIGHT: {
                            unsigned x = console_get_cursor_x(g_console);
                            if(x < console_get_width(g_console))
                                ++x;
                            console_cursor_goto_xy(g_console, x, console_get_cursor_y(g_console));
                        }
                        break;
                    case SDLK_RETURN:
                        console_print_char(g_console, '\n');
                        break;
                    }
                    break;
                }
                break;
            }
        }
        render();
    }
    shutDown();
    return 0;
}

static void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    g_screenSurface = SDL_SetVideoMode(SCREEN_WIDTH,
                                       SCREEN_HEIGHT,
                                       SCREEN_BPP,
                                       SDL_HWSURFACE | SDL_FULLSCREEN | SDL_NOFRAME);

    if (g_screenSurface == NULL) {
        printf("Unable to set %dx%dx%d video: %s\n",
                SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_GetError());
        exit(1);
    }

    SDL_ShowCursor(0);

    g_console = console_alloc(SCREEN_WIDTH, SCREEN_HEIGHT);
    console_set_font(g_console, FONT_8x16);
    render_init(g_screenSurface, g_console);
    console_set_callback(g_console, render_callback);
    console_clear(g_console);
}

static void shutDown() {
    render_done();
    console_set_callback(g_console, 0);
    console_free(g_console);
    SDL_FreeSurface(g_screenSurface);
    SDL_Quit();
}

static void render_callback(console_t console, console_update_t * u) {
    switch(u->type) {
    case CONSOLE_UPDATE_CHAR: {
            render_char(console,
                        g_screenSurface,
                        u->data.u_char.x,
                        u->data.u_char.y,
                        u->data.u_char.c);
        }
        break;
    case CONSOLE_UPDATE_ROWS:
        break;
    case CONSOLE_UPDATE_SCROLL: {
            SDL_Rect src;
            SDL_Rect dst;
            unsigned sw = console_get_width(console);
            unsigned sh = console_get_height(console);
            unsigned w = console_get_char_width(console);
            unsigned h = console_get_char_height(console);
            src.x = dst.x = 0;
            src.w = dst.w = w * sw;
            src.h = dst.h = u->data.u_scroll.n * h;
            src.y = u->data.u_scroll.y2 * h;
            dst.y = u->data.u_scroll.y1 * h;
            SDL_BlitSurface(g_screenSurface, &src, g_screenSurface, &dst);
            dst.y = (sh * h) - (console_get_height(console) - u->data.u_scroll.n) * h;
            dst.h = (u->data.u_scroll.y2 - u->data.u_scroll.y1) * h;
            SDL_FillRect(g_screenSurface, &dst, render_get_background_color(console));
            SDL_UpdateRect(g_screenSurface, 0, 0, 0, 0);
        }
        break;
    case CONSOLE_UPDATE_CURSOR_VISIBILITY: {
            if(u->data.u_cursor.cursor_visible) {
                render_cursor(console,
                              g_screenSurface,
                              u->data.u_cursor.x,
                              u->data.u_cursor.y);
            } else {
                render_char(console,
                            g_screenSurface,
                            u->data.u_cursor.x,
                            u->data.u_cursor.y,
                            console_get_char_at(console,
                                                u->data.u_cursor.x,
                                                u->data.u_cursor.y));
            }
        }
        break;
    case CONSOLE_UPDATE_CURSOR_POSITION: {
            render_char(console,
                        g_screenSurface,
                        u->data.u_cursor.x,
                        u->data.u_cursor.y,
                        console_get_char_at(console,
                                            u->data.u_cursor.x,
                                            u->data.u_cursor.y));
            if(u->data.u_cursor.cursor_visible) {
                render_cursor(console,
                              g_screenSurface,
                              console_get_cursor_x(console),
                              console_get_cursor_y(console));
            }
        }
        break;
    }
}

static void render(void) {
    console_blink_cursor(g_console);
    SDL_UpdateRect(g_screenSurface, 0,0,0,0);
}
