#include "timeline.h"
#include "common.h"
#include <stdio.h>

void render_timeline(SDL_Renderer *ren, TTF_Font *font, interrupt_t *history, int count, int offset) {
    int start_x = 80;   
    int base_y = 350;   
    int tick_w = 40;    
    int row_h = 60;     
    int accumulated_ticks = 0; 

    // 1. 가이드라인 및 우선순위 라벨 (배경 고정)
    for (int i = 0; i < 5; i++) {
        int y = base_y - (i * row_h);
        SDL_SetRenderDrawColor(ren, 50, 50, 55, 255);
        SDL_RenderDrawLine(ren, start_x, y + 25, 1250, y + 25);
        
        char p_label[10];
        sprintf(p_label, "P%d", i);
        SDL_Rect label_rect = { start_x - 65, y, 50, 50 };
        draw_text_centered(ren, font, p_label, label_rect, (SDL_Color){150, 150, 150, 255});
    }

    // 2. 하단 시간 축 숫자 표시 (Time Axis - 스크롤 적용)
    int temp_ticks = 0;
    for (int i = 0; i <= count; i++) {
        int num_x = start_x + (temp_ticks * tick_w) - offset;
        
        if (num_x >= start_x && num_x < 1250) {
            char time_buf[16];
            sprintf(time_buf, "%d", temp_ticks);
            SDL_Rect time_rect = { num_x - 15, base_y + 45, 30, 20 };
            draw_text_centered(ren, font, time_buf, time_rect, (SDL_Color){130, 130, 140, 255});
            
            SDL_SetRenderDrawColor(ren, 100, 100, 110, 255);
            SDL_RenderDrawLine(ren, num_x, base_y + 30, num_x, base_y + 40);
        }
        if (i < count) temp_ticks += history[i].running_time;
    }

    // 3. 인터럽트 블록 렌더링 (스크롤 적용)
    accumulated_ticks = 0;
    for (int i = 0; i < count; i++) {
        int block_x = start_x + (accumulated_ticks * tick_w) - offset;
        int block_w = history[i].running_time * tick_w;
        int block_y = base_y - (history[i].priority * row_h);
        
        if (block_x + block_w > start_x && block_x < 1250) {
            SDL_Rect rect = { block_x, block_y, block_w - 2, 50 };
            set_priority_color(ren, history[i].priority);
            SDL_RenderFillRect(ren, &rect);
            
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &rect);

            char info[128];
            sprintf(info, "%s (%d)", history[i].irq_case, history[i].running_time);
            draw_text_centered(ren, font, info, rect, (SDL_Color){255, 255, 255, 255});
        }
        accumulated_ticks += history[i].running_time;
    }
}