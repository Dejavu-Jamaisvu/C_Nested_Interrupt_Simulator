// [ui_queue.h]
#ifndef UI_QUEUE_H
#define UI_QUEUE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "types.h"

void render_priority_queue(SDL_Renderer* renderer, TTF_Font* font, priority_queue_t* queue_instance, int offset_x, int offset_y);

#endif