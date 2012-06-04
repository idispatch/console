#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "console.h"
#include "render.h"

static SDL_Surface *g_screenSurface = NULL;
console_t g_console = NULL;

const int UPDATE_INTERVAL = 10;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

int main(int argc, char *argv[]);
static void init();
static void shutDown();
static Uint32 timeLeft();
static void render();

int main(int argc, char *argv[]) {
    init();

    bool bDone = false;

    SDL_FillRect(g_screenSurface, NULL, SDL_MapRGB(g_screenSurface->format, 0, 0, 0));
    SDL_UpdateRect(g_screenSurface, 0, 0, 0, 0);

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

    g_screenSurface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (g_screenSurface == NULL) {
        printf("Unable to set %dx%dx%d video: %s\n",
                SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_GetError());
        exit(1);
    }

    g_console = console_alloc(SCREEN_WIDTH, SCREEN_HEIGHT);
    font_render_init(g_screenSurface, g_console);
    console_print_string(g_console, "Hello World!\n0123456789");
}

static void shutDown() {
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

static void render(void) {
    SDL_Delay(timeLeft());

    Uint32 black = SDL_MapRGB(g_screenSurface->format, 0, 0, 0);
    SDL_Rect destRect;
    destRect.x = 0;
    destRect.w = SCREEN_WIDTH;
    destRect.h = SCREEN_HEIGHT;

    static int nn = 0;

    char str[200];
    unsigned num_lines = console_get_height(g_console);
    unsigned char_height = console_get_char_height(g_console);
    unsigned y;
    for(y = 0; y < num_lines; y++) {
        destRect.y = y * char_height;
        SDL_FillRect(g_screenSurface, &destRect, black);
        console_get_string_at(g_console, 0, y, str, sizeof(str));
        font_render_string(g_console, g_screenSurface, 0, destRect.y, str);
    }
    SDL_UpdateRect(g_screenSurface, 0, 0, 0, 0);

    char hh[60];
    sprintf(hh, "test %d ", nn++);
    console_print_string(g_console, hh);

    SDL_Flip(g_screenSurface);
}
