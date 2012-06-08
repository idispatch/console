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

int main(int argc, char *argv[]) {
    init();
    bool bDone = false;
    console_print_string(g_console, "0123456789");
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
                    static unsigned char c = 0;
                    console_print_char(g_console, (char)(event.key.keysym.unicode & 0xff));
                    console_set_attribute(g_console, c++);
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
    render_init(g_console, g_screenSurface);
    console_set_callback(g_console, console_render_callback, g_screenSurface);
    console_clear(g_console);
    console_set_attribute(g_console, 1);
}

static void shutDown() {
    render_done();
    console_set_callback(g_console, NULL, NULL);
    console_free(g_console);
    SDL_FreeSurface(g_screenSurface);
    SDL_Quit();
}

static void render(void) {
    console_blink_cursor(g_console);
    SDL_UpdateRect(g_screenSurface, 0,0,0,0);
}
