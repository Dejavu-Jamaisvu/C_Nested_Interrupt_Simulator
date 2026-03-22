#include "timeline.h"
#include "common.h"
#include <stdio.h>

void render_timeline(SDL_Renderer *ren, TTF_Font *font, interrupt_t *history, int count, int offset) {
    int start_x = 80;   
    int screen_w = 1280; 
    int end_x = 1250;    
    int base_y = 350;   
    int tick_w = 40;    
    int row_h = 60;     

    // 데이터 끝을 화면 90% 지점에 맞추기 위한 계산
    int target_x = (int)(screen_w * 0.9); 
    int total_data_w = 0;
    for (int i = 0; i < count; i++) total_data_w += (history[i].running_time * tick_w);
    
    int timeline_base_x = target_x - total_data_w - offset;

    // 1. 가이드라인 및 P0~P4 라벨
    for (int i = 0; i <= 4; i++) {
        int y = base_y - (i * row_h);
        SDL_SetRenderDrawColor(ren, 50, 50, 55, 255);
        SDL_RenderDrawLine(ren, start_x, y + 25, end_x, y + 25);
        
        char p_label[10];
        sprintf(p_label, "P%d", i);
        SDL_Rect label_rect = { start_x - 65, y, 50, 50 };
        draw_text_centered(ren, font, p_label, label_rect, (SDL_Color){150, 150, 150, 255});
    }

    // 2. 시간 축 및 블록 렌더링
    int current_x = timeline_base_x;
    int time_counter = 0; 

    for (int i = 0; i < count; i++) {
        int block_w = history[i].running_time * tick_w;
        int block_y = base_y - (history[i].priority * row_h);

        // 시간 축 숫자
        if (current_x >= start_x && current_x < end_x) {
            char time_buf[16];
            sprintf(time_buf, "%d", time_counter);
            SDL_Rect time_rect = { current_x - 15, base_y + 45, 30, 20 };
            draw_text_centered(ren, font, time_buf, time_rect, (SDL_Color){130, 130, 140, 255});
            
            SDL_SetRenderDrawColor(ren, 100, 100, 110, 255);
            SDL_RenderDrawLine(ren, current_x, base_y + 30, current_x, base_y + 40);
        }

        // 블록 그리기
        if (current_x + block_w > start_x && current_x < end_x) {
            int draw_x = (current_x < start_x) ? start_x : current_x;
            int draw_w = (current_x < start_x) ? (block_w - (start_x - current_x)) : block_w;
            if (draw_x + draw_w > end_x) draw_w = end_x - draw_x;

            SDL_Rect rect = { draw_x, block_y, draw_w - 2, 50 };
            set_priority_color(ren, history[i].priority);
            SDL_RenderFillRect(ren, &rect);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &rect);

            // [수정] 블록 내부 텍스트 표시
            // 경고 방지를 위해 버퍼를 128로 확장
            if (draw_w > 70) {
                char line1[128], line2[64];
                sprintf(line1, "%s", history[i].irq_case);
                sprintf(line2, "(%dms)", history[i].running_time);

                SDL_Rect t_rect = { rect.x, rect.y + 5, rect.w, 20 };
                SDL_Rect b_rect = { rect.x, rect.y + 25, rect.w, 20 };
                
                draw_text_centered(ren, font, line1, t_rect, (SDL_Color){255, 255, 255, 255});
                draw_text_centered(ren, font, line2, b_rect, (SDL_Color){255, 255, 255, 255});
            } 
            else if (draw_w > 30) {
                draw_text_centered(ren, font, history[i].irq_case, rect, (SDL_Color){255, 255, 255, 255});
            }
        }
        
        current_x += block_w;
        time_counter += history[i].running_time;
    }

    // 마지막 시간 표시
    if (current_x >= start_x && current_x < end_x) {
        char time_buf[16];
        sprintf(time_buf, "%d", time_counter);
        SDL_Rect time_rect = { current_x - 15, base_y + 45, 30, 20 };
        draw_text_centered(ren, font, time_buf, time_rect, (SDL_Color){130, 130, 140, 255});
    }
}