#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "console.h"
#include "render.h"

static SDL_Surface *g_screenSurface = NULL;
static console_t g_console = NULL;

static const int UPDATE_INTERVAL = 5;

static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 600;
static const int SCREEN_BPP = 32;

static void init();
static void shutDown();
static Uint32 timeLeft();
static void render();
static void render_callback(console_t console, console_update_t * u);

int main(int argc, char *argv[]) {
    init();

    bool bDone = false;

    //SDL_FillRect(g_screenSurface, NULL, SDL_MapRGB(g_screenSurface->format, 0, 0, 0));
    //SDL_UpdateRect(g_screenSurface, 0, 0, 0, 0);

    while (bDone == false) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                bDone = true;
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

    g_console = console_alloc(SCREEN_WIDTH, SCREEN_HEIGHT);
    font_render_init(g_screenSurface, g_console);
    console_set_callback(g_console, render_callback);
    console_clear(g_console);
    console_print_string(g_console, "Hello World!\n0123456789");
}

static void shutDown() {
    font_render_done();
    console_set_callback(g_console, 0);
    console_free(g_console);
    SDL_FreeSurface(g_screenSurface);
    SDL_Quit();
}

static Uint32 timeLeft(void) {
    static Uint32 timeToNextUpdate = 0;
    Uint32 currentTime;

    currentTime = SDL_GetTicks();

    if (timeToNextUpdate <= currentTime) {
        timeToNextUpdate = currentTime + UPDATE_INTERVAL;
        return 0;
    }

    return (timeToNextUpdate - currentTime);
}

static void render_callback(console_t console, console_update_t * u) {
    switch(u->type) {
    case CONSOLE_UPDATE_CHAR: {
            unsigned w = console_get_char_width(console);
            unsigned h = console_get_char_height(console);
            unsigned x = u->data.u_char.x * w;
            unsigned y = u->data.u_char.y * h;
            font_render_char(console,
                             g_screenSurface,
                             x,
                             y,
                             u->data.u_char.c);
        }
        break;
    case CONSOLE_UPDATE_ROWS:
        /*
        printf("CONSOLE_UPDATE_ROWS: (%d,%d)x(%d,%d)\n", u->data.u_rows.x1, u->data.u_rows.y1, u->data.u_rows.x2, u->data.u_rows.y2);
        fflush(stdout);
        */
        break;
    case CONSOLE_UPDATE_SCROLL: {
            SDL_Rect src;
            SDL_Rect dst;
            unsigned h = console_get_char_height(console);
            src.x = dst.x = 0;
            src.w = dst.w = SCREEN_WIDTH;
            src.h = dst.h = u->data.u_scroll.n * h;
            src.y = u->data.u_scroll.y2 * h;
            dst.y = u->data.u_scroll.y1 * h;
            SDL_BlitSurface(g_screenSurface, &src, g_screenSurface, &dst);
            dst.y = SCREEN_HEIGHT - (console_get_height(console) - u->data.u_scroll.n) * h;
            dst.h = (u->data.u_scroll.y2 - u->data.u_scroll.y1) * h;
            SDL_FillRect(g_screenSurface, &dst, font_render_get_background_color(console));
            SDL_UpdateRect(g_screenSurface, 0, 0, 0, 0);
        }
        break;
    }
}

static void render(void) {
    static char c = 0;
    console_print_char(g_console, c++);
    //if(c>'z') c='A';
    //console_print_string(g_console, "Hello World! ");
    //SDL_UpdateRects(dst, 1, &drect);
    //SDL_UpdateRect(g_screenSurface, 0,0,0,0);
}
