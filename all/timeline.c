//timeline.c
#include "timeline.h"

void render_timeline(SDL_Renderer *ren, TTF_Font *font, interrupt_t *history, int count) {
    int start_x = 80;   // 타임라인 시작점
    int base_y = 350;   // P0 기준 높이
    int row_h = 60;     // 층간 간격
    int tick_w = 35;    // 1ms당 너비 (35px)

    // 1. 가이드라인 및 라벨 출력
    for (int i = 0; i < 5; i++) {
        int y = base_y - (i * row_h);
        SDL_SetRenderDrawColor(ren, 50, 50, 55, 255);
        SDL_RenderDrawLine(ren, start_x, y + 25, 1200, y + 25);
        
        char p_label[10];
        sprintf(p_label, "P%d", i);
        SDL_Rect label_rect = { start_x - 60, y, 50, 50 };
        draw_text_centered(ren, font, p_label, label_rect, (SDL_Color){150, 150, 150, 255});
    }

    // 2. 누적 시간을 계산하며 블록 배치
    int accumulated_ticks = 0; 

    for (int i = 0; i < count; i++) {
        // 시작점 = 전체 시작점 + (이전까지의 총 실행 시간 * 단위 너비)
        int block_x = start_x + (accumulated_ticks * tick_w);
        int block_w = history[i].running_time * tick_w;
        int block_y = base_y - (history[i].priority * row_h);
        
        SDL_Rect rect = { block_x, block_y, block_w - 2, 50 };

        // 우선순위 색상 적용 (common.h)
        set_priority_color(ren, history[i].priority);
        SDL_RenderFillRect(ren, &rect);
        
        // 테두리
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &rect);

        // 텍스트: "이름 (시간)"
        char info[128];
        sprintf(info, "%s (%d)", history[i].irq_case, history[i].running_time);
        draw_text_centered(ren, font, info, rect, (SDL_Color){255, 255, 255, 255});

        // 다음 블록을 위해 현재 실행 시간을 더함
        accumulated_ticks += history[i].running_time;
    }
}