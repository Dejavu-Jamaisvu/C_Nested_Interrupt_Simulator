#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

typedef struct _priority_queue {
    interrupt_t data[MAX_QUEUE_SIZE];
    int size;
} priority_queue_t;

// [수정] 마지막에 int offset_x 인자를 추가합니다.
void render_queue(SDL_Renderer *ren, TTF_Font *font, priority_queue_t *pq, int offset_x, int offset_y);

#endif