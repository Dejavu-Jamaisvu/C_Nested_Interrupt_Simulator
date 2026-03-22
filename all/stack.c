// stack.c
 
#include "stack.h"

void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns) {
    // 하단 좌측 영역 박스
    SDL_Rect area = { 50, 420, 350, 360 }; 
    SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 70, 70, 80, 255);
    SDL_RenderDrawRect(ren, &area);

    // 타이틀
    SDL_Rect title_box = { area.x, area.y + 10, area.w, 30 };
    draw_text_centered(ren, font, "NESTED STACK (LIFO)", title_box, (SDL_Color){255, 255, 100, 255});

    if (ns->data_count <= 0) return;

    int block_w = 280;
    int block_h = 45;
    int gap = 8;
    // 박스 바닥에서 약간 띄운 지점부터 시작
    int start_y_bottom = area.y + area.h - 20; 

    for (int i = 0; i < ns->data_count; i++) {
        SDL_Rect block = {
            area.x + (area.w - block_w) / 2,     // 가로 중앙 정렬
            start_y_bottom - (i + 1) * (block_h + gap), // 아래서부터 i번째 층
            block_w,
            block_h
        };

        set_priority_color(ren, ns->stack[i].priority);
        SDL_RenderFillRect(ren, &block);
        
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &block);

        char buf[128];
        sprintf(buf, "[P%d] %s", ns->stack[i].priority, ns->stack[i].irq_case);
        draw_text_centered(ren, font, buf, block, (SDL_Color){255, 255, 255, 255});
    }
}