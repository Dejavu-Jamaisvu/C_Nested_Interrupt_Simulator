// [ui_stack.h]
#ifndef UI_STACK_H
#define UI_STACK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "types.h"

void render_nested_stack(SDL_Renderer* renderer, TTF_Font* font, nested_stack_t* stack_instance, int offset_y);

#endif