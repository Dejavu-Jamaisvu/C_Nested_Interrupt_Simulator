#ifndef TIMELINE_H
#define TIMELINE_H

#include "common.h"

// 마지막에 int offset이 꼭 있어야 합니다!
void render_timeline(SDL_Renderer *ren, TTF_Font *font, interrupt_t *history, int count, int offset);

#endif