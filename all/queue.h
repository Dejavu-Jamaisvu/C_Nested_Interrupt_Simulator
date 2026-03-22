// queue.h
#include "common.h"

typedef struct _priority_queue {
    interrupt_t data[MAX_QUEUE_SIZE];
    int size;
} priority_queue_t;

void render_queue(SDL_Renderer *ren, TTF_Font *font, priority_queue_t *pq);