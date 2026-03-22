#include "stack.h"
#include "common.h"
#include <stdio.h>

void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns, int offset_y) {
    SDL_Rect area = { 50, 420, 350, 360 }; 
    SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 70, 70, 80, 255);
    SDL_RenderDrawRect(ren, &area);

    SDL_Rect title_box = { area.x, area.y + 10, area.w, 30 };
    draw_text_centered(ren, font, "NESTED STACK (LIFO)", title_box, (SDL_Color){255, 255, 100, 255});

    if (ns->data_count <= 0) return;

    // [수정] 압축 로직 제거: 블록 크기를 고정하여 가독성 확보
    int block_w = 280;
    int block_h = 45; 
    int gap = 10;
    
    // [중요] 시작점 계산: 바닥 지점에 offset_y를 더해 전체가 같이 움직이게 함
    int start_y_bottom = (area.y + area.h - 25) + offset_y; 

    for (int i = 0; i < ns->data_count; i++) {
        int y_pos = start_y_bottom - (i + 1) * (block_h + gap);

        // [클리핑] 타이틀바 아래 영역에 있을 때만 렌더링
        if (y_pos > area.y + 40 && y_pos < area.y + area.h - 10) {
            SDL_Rect block = {
                area.x + (area.w - block_w) / 2,
                y_pos,
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
}