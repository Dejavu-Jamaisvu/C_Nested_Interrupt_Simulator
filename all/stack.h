// stack.h
#include "common.h"

typedef struct _nested_stack {
    interrupt_t stack[MAX_STACK_SIZE];
    void (*push)(interrupt_t);
    interrupt_t (*pop)();
    int (*is_empty)();
    int (*is_full)();
    int data_count;
} nested_stack_t;

void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns);