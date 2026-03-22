#ifndef STACK_H
#define STACK_H

#include "common.h"

typedef struct _nested_stack {
    interrupt_t stack[MAX_STACK_SIZE];
    void (*push)(interrupt_t);
    interrupt_t (*pop)();
    int (*is_empty)();
    int (*is_full)();
    int data_count;
} nested_stack_t;

// [수정] 마지막에 int offset_y 인자를 추가합니다.
void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns, int offset_y);

#endif