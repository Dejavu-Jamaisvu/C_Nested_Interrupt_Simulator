// [ui_stack.c]
#include "ui_stack.h"
#include "ui_main.h"
#include <stdio.h>
#include <string.h>

void render_nested_stack(SDL_Renderer* renderer, TTF_Font* font, nested_stack_t* stack_instance, int offset_y) {
    // 1. 배경 영역 설정
    SDL_Rect background_area = { 50, 450, 400, 400 }; 
    SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
    SDL_RenderFillRect(renderer, &background_area);
    
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderDrawRect(renderer, &background_area);

    // 2. 타이틀 렌더링
    SDL_Rect title_box = { background_area.x, background_area.y + 10, background_area.w, 30 };
    SDL_Color yellow_color = {255, 255, 100, 255};
    draw_text_centered(renderer, font, "NESTED STACK (STRESS TEST)", title_box, yellow_color);

    if (stack_instance->current_data_count <= 0) {
        return;
    }

    // 3. 렌더링 제한 구역(Clipping) 설정
    SDL_Rect clipping_area = { background_area.x + 5, background_area.y + 45, background_area.w - 10, background_area.h - 55 };
    SDL_RenderSetClipRect(renderer, &clipping_area);

    // 4. 수치 설정
    int block_width = 340;
    int block_height = 45;
    int block_gap = 10;
    int step_size = block_height + block_gap;
    int visible_height = background_area.h - 55; // 실제 블록들이 보일 수 있는 최대 높이

    // --- 핵심: 자동 스크롤(TOP 추적) 계산 ---
    int total_stack_height = stack_instance->current_data_count * step_size;
    int auto_scroll_offset = 0;

    // 데이터가 가시 영역을 넘어가면, TOP이 제목 바로 아래(상단)에 오도록 offset을 계산
    if (total_stack_height > visible_height) {
        auto_scroll_offset = total_stack_height - visible_height;
    }

    // 사용자 수동 스크롤(offset_y) + 시스템 자동 추적(auto_scroll_offset)
    int final_offset = offset_y + auto_scroll_offset;

    // 기준점: 배경 영역의 가장 아래쪽 좌표
    int base_bottom_y = background_area.y + background_area.h - 20;

    // 5. 스택 렌더링 루프
    for (int i = 0; i < stack_instance->current_data_count; i++) {
        // 바닥에서부터 위로 쌓아 올리는 좌표 계산
        int current_y_position = base_bottom_y - (i + 1) * step_size + final_offset;

        // 클리핑 영역 안에 있는 경우만 그리기
        if (current_y_position + block_height > background_area.y + 45 && current_y_position < background_area.y + background_area.h) {
            SDL_Rect task_block = { background_area.x + (background_area.w - block_width) / 2, current_y_position, block_width, block_height };
            
            // 색상 지정
            set_priority_color(renderer, stack_instance->stack_array[i].task_priority);
            SDL_RenderFillRect(renderer, &task_block);
            
            // TOP 강조 (노란색 굵은 테두리)
            if (i == stack_instance->current_data_count - 1) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
                SDL_RenderDrawRect(renderer, &task_block);
                SDL_Rect inner_rect = { task_block.x + 1, task_block.y + 1, task_block.w - 2, task_block.h - 2 };
                SDL_RenderDrawRect(renderer, &inner_rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &task_block);
            }

            // 텍스트 출력
            char display_buffer[256];
            snprintf(display_buffer, sizeof(display_buffer), "[P%d] %s (%d ms)", 
                    stack_instance->stack_array[i].task_priority, 
                    stack_instance->stack_array[i].task_name, 
                    stack_instance->stack_array[i].remaining_running_time);

            if (i == stack_instance->current_data_count - 1) {
                strcat(display_buffer, " [TOP]");
            }
            else if (i == 0) {
                strcat(display_buffer, " [BOTTOM]");
            }

            SDL_Color white_color = {255, 255, 255, 255};
            draw_text_centered(renderer, font, display_buffer, task_block, white_color);
        }
    }
    
    SDL_RenderSetClipRect(renderer, NULL);
}