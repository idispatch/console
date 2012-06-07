#ifndef RENDER_H_
#define RENDER_H_

#include <SDL.h>
#include "console.h"

void render_init(SDL_Surface * dst, console_t console);
void render_done();
void render_cursor(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y);
void render_char(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char c);
void render_string(console_t console, SDL_Surface * dst, Sint16 x, Sint16 y, char * str);
Uint32 render_get_background_color(console_t console);
Uint32 render_get_foreground_color(console_t console);
#ifdef _DEBUG
void render_font_surface(console_t console, SDL_Surface * dst);
#endif

#endif /* RENDER_H_ */
