#ifndef RENDER_H_
#define RENDER_H_

#include <SDL.h>
#include "console.h"

void render_init(console_t console, SDL_Surface * dst);
void render_done();
void console_render_callback(console_t console, console_update_t * u, void * data);

#endif /* RENDER_H_ */
