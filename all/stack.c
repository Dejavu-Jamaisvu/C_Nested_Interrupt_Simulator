#include "stack.h"
#include "common.h"
#include <stdio.h>

void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns, int offset_y) {
    // 1. 스택 전체 영역 (배경/테두리)
    SDL_Rect area = { 50, 420, 350, 360 }; 
    SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 70, 70, 80, 255);
    SDL_RenderDrawRect(ren, &area);

    // 타이틀 (클리핑 영향을 안 받게 먼저 그림)
    SDL_Rect title_box = { area.x, area.y + 10, area.w, 30 };
    draw_text_centered(ren, font, "NESTED STACK (LIFO)", title_box, (SDL_Color){255, 255, 100, 255});

    if (ns->data_count <= 0) return;

    // 2. [큐와 동일하게 만드는 핵심] 클리핑 영역 설정
    // 타이틀바 아래부터 박스 바닥까지만 "그림이 그려지도록" 제한합니다.
    // 이렇게 하면 블록이 경계선에 걸쳐도 픽셀 단위로 잘려서 보입니다.
    SDL_Rect clip_area = { area.x + 2, area.y + 45, area.w - 4, area.h - 55 };
    SDL_RenderSetClipRect(ren, &clip_area);

    int block_w = 280, block_h = 45, gap = 10;
    int step = block_h + gap;
    
    // 기준점: 최상단(Top)이 박스 상단에 오도록 설정
    int top_y = area.y + 55;

    for (int i = 0; i < ns->data_count; i++) {
        int data_idx = (ns->data_count - 1) - i;
        int y_pos = top_y + (i * step) + offset_y;

        // 이제 if문으로 블록 전체를 검사하지 않아도 ClipRect가 알아서 잘라줍니다.
        // 다만 성능을 위해 화면에서 너무 먼 곳은 그리지 않도록 대략적인 범위만 잡습니다.
        if (y_pos > area.y - 100 && y_pos < area.y + area.h + 100) {
            SDL_Rect block = { area.x + (area.w - block_w) / 2, y_pos, block_w, block_h };

            set_priority_color(ren, ns->stack[data_idx].priority);
            SDL_RenderFillRect(ren, &block);
            
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &block);

            char buf[128];
            sprintf(buf, "[P%d] %s", ns->stack[data_idx].priority, ns->stack[data_idx].irq_case);
            draw_text_centered(ren, font, buf, block, (SDL_Color){255, 255, 255, 255});
        }
    }

    // 3. 클리핑 해제 (매우 중요: 큐나 다른 UI가 안 잘리게 함)
    SDL_RenderSetClipRect(ren, NULL);
}