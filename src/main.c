#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

static SDL_Surface *g_screenSurface = NULL;
static SDL_Surface *g_donutSurface = NULL;

const int UPDATE_INTERVAL = 10;

int main(int argc, char *argv[]);
static void init();
static void shutDown();
static void loadBMP();
static Uint32 timeLeft();
static void render();

int main(int argc, char *argv[]) {
    init();

    loadBMP();

    bool bDone = false;

    while (bDone == false) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                bDone = true;

            /*if( event.type == SDL_KEYDOWN )
             {
             if( event.key.keysym.sym == SDLK_ESCAPE )
             bDone = true;
             }*/
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

    g_screenSurface = SDL_SetVideoMode(1024, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (g_screenSurface == NULL) {
        printf("Unable to set 1024x600 video: %s\n", SDL_GetError());
        exit(1);
    }
}

static void shutDown() {
    SDL_FreeSurface(g_donutSurface);
    SDL_FreeSurface(g_screenSurface);
    SDL_Quit();
}

static void loadBMP() {
    // Load the BMP file into a surface
    // char current_dir[260];
    // getcwd(current_dir, sizeof(current_dir));
    const char * home_path = getenv("HOME");
    char asset_path[256];
    snprintf(asset_path, sizeof(asset_path), "%s/../app/native/donut.bmp", home_path);

    g_donutSurface = SDL_LoadBMP(asset_path);

    if (g_donutSurface == NULL) {
        fprintf(stderr, "Couldn't load \"assets/donut.bmp\" (%s)", SDL_GetError());
        return;
    }

    SDL_SetColorKey(g_donutSurface, SDL_SRCCOLORKEY, SDL_MapRGB(g_donutSurface->format, 0, 0, 0));
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

    SDL_FillRect(g_screenSurface, NULL, SDL_MapRGB(g_screenSurface->format, 0, 0, 255));

    static int nPosition = 0;
    static int nFrame = 0;

    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Build a source SDL_Rect which will copy only a small portion of the texture.
    srcRect.x = ((nFrame % 5) * 64);
    srcRect.y = ((nFrame / 5) * 64);
    srcRect.w = 64;
    srcRect.h = 64;

    destRect.x = nPosition;
    destRect.y = 200;
    destRect.w = 64;
    destRect.h = 64;

    SDL_BlitSurface(g_donutSurface, &srcRect, g_screenSurface, &destRect);

    // Update the changed portion of the screen
    SDL_UpdateRects(g_screenSurface, 1, &destRect);

    //
    // Increment the sprite's frame number. Our sprite's animation sequence
    // consists of 30 frames (0-29).
    //

    ++nFrame;
    if (nFrame > 29)
        nFrame = 0;

    ++nPosition;
    if (nPosition > 640)
        nPosition = 0;

    SDL_Flip(g_screenSurface);
}
