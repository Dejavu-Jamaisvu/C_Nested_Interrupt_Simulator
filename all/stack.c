// stack.c
#include "stack.h"

void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns) {
    SDL_Rect area = { 50, 420, 350, 360 };
    SDL_SetRenderDrawColor(ren, 40, 40, 45, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 100, 100, 110, 255);
    SDL_RenderDrawRect(ren, &area);

    draw_text_centered(ren, font, "NESTED STACK (LIFO)", (SDL_Rect){50, 430, 350, 30}, (SDL_Color){255, 255, 100, 255});

    for (int i = 0; i < ns->data_count; i++) {
        SDL_Rect block = { 75, 720 - (i * 55), 260, 50 };
        SDL_SetRenderDrawColor(ren, (ns->stack[i].priority == 0) ? 70 : 180, 80, 80, 255);
        SDL_RenderFillRect(ren, &block);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &block);
        
        char buf[120]; sprintf(buf, "[P%d] %s", ns->stack[i].priority, ns->stack[i].irq_case);
        draw_text_centered(ren, font, buf, block, (SDL_Color){255, 255, 255, 255});
    }
}