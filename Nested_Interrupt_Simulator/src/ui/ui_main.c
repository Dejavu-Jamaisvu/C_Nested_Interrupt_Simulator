// [ui_main.c]
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "types.h"
#include "ui_main.h"

// 1. 우선순위별 색상 설정 (기존 유지)
void set_priority_color(SDL_Renderer* renderer, int priority_level) {
    if (priority_level == 0) SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
    else if (priority_level == 1) SDL_SetRenderDrawColor(renderer, 200, 100, 50, 255);
    else if (priority_level == 2) SDL_SetRenderDrawColor(renderer, 200, 180, 50, 255);
    else if (priority_level == 3) SDL_SetRenderDrawColor(renderer, 50, 180, 50, 255);
    else SDL_SetRenderDrawColor(renderer, 50, 100, 200, 255);
}

// 2. 텍스트 중앙 정렬 함수 (기존 유지)
void draw_text_centered(SDL_Renderer* renderer, TTF_Font* font, const char* text_string, SDL_Rect bounding_box, SDL_Color text_color) {
    if (text_string == NULL || text_string[0] == '\0') return;
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text_string, text_color);
    if (!text_surface) return;
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    
    int center_x = bounding_box.x + (bounding_box.w - text_surface->w) / 2;
    int center_y = bounding_box.y + (bounding_box.h - text_surface->h) / 2;
    SDL_Rect target_rect = { center_x, center_y, text_surface->w, text_surface->h };
    
    SDL_RenderCopy(renderer, text_texture, NULL, &target_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

// 3. 타임라인 렌더링 (기존 텍스트 표기 방식 유지 + 90% 스크롤)
void render_timeline(SDL_Renderer* renderer, TTF_Font* font, interrupt_task_t* history_array, int total_count, int offset_x) {
    int start_x_position = 80;   
    int screen_width = 1600;  
    int end_x_position = 1550;   
    int base_y_position = 380;   
    int tick_width = 40;   
    int row_height = 60;     

    // [로직 변경] 90% 지점 자동 스크롤 계산
    int available_width = end_x_position - start_x_position;
    int limit_x_threshold = (int)(available_width * 0.90); 
    
    int total_data_width = 0;
    for (int i = 0; i < total_count; i++) {
        total_data_width += (history_array[i].remaining_running_time * tick_width);
    }

    int auto_offset_x = 0;
    if (total_data_width > limit_x_threshold) {
        auto_offset_x = total_data_width - limit_x_threshold;
    }

    // 최종 시작 위치 (80px 기준)
    int timeline_base_x_position = start_x_position - auto_offset_x + offset_x;

    // 가이드라인 및 P0~P4 라벨 출력
    for (int level = 0; level <= 4; level++) {
        int y_pos = base_y_position - (level * row_height);
        SDL_SetRenderDrawColor(renderer, 50, 50, 55, 255);
        SDL_RenderDrawLine(renderer, start_x_position, y_pos + 25, end_x_position, y_pos + 25);
        
        char label[10];
        snprintf(label, sizeof(label), "P%d", level);
        SDL_Rect label_rect = { start_x_position - 65, y_pos, 50, 50 };
        SDL_Color gray = {150, 150, 150, 255};
        draw_text_centered(renderer, font, label, label_rect, gray);
    }

    int current_x = timeline_base_x_position;
    int time_counter = 0; 

    for (int i = 0; i < total_count; i++) {
        int block_w = history_array[i].remaining_running_time * tick_width;
        int block_y = base_y_position - (history_array[i].task_priority * row_height);

        // [기존 유지] 시간 눈금 및 숫자 표시
        if (current_x >= start_x_position && current_x <= end_x_position) {
            char time_buf[16];
            snprintf(time_buf, sizeof(time_buf), "%d", time_counter);
            SDL_Rect time_rect = { current_x - 15, base_y_position + 45, 30, 20 };
            SDL_Color light_gray = {130, 130, 140, 255};
            draw_text_centered(renderer, font, time_buf, time_rect, light_gray);
            
            SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
            SDL_RenderDrawLine(renderer, current_x, base_y_position + 30, current_x, base_y_position + 40);
        }

        // 블록 렌더링 및 기존 Clipping 로직
        if (current_x + block_w > start_x_position && current_x < end_x_position) {
            int draw_x = (current_x < start_x_position) ? start_x_position : current_x;
            int draw_end_x = (current_x + block_w > end_x_position) ? end_x_position : current_x + block_w;
            int draw_w = draw_end_x - draw_x;

            if (draw_w > 0) {
                SDL_Rect task_rect = { draw_x, block_y, draw_w - 2, 50 };
                set_priority_color(renderer, history_array[i].task_priority);
                SDL_RenderFillRect(renderer, &task_rect);
                
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &task_rect);

                SDL_Color white = {255, 255, 255, 255};

                // [기존 내용 유지] 너비가 충분할 때 이름과 시간을 두 줄로 출력
                if (draw_w > 70) {
                    char first_line[128], second_line[64];
                    snprintf(first_line, sizeof(first_line), "%s", history_array[i].task_name);
                    snprintf(second_line, sizeof(second_line), "(%d ms)", history_array[i].remaining_running_time);

                    SDL_Rect r1 = { task_rect.x, task_rect.y + 5, task_rect.w, 20 };
                    SDL_Rect r2 = { task_rect.x, task_rect.y + 25, task_rect.w, 20 };
                    
                    draw_text_centered(renderer, font, first_line, r1, white);
                    draw_text_centered(renderer, font, second_line, r2, white);
                } 
                else if (draw_w > 30) {
                    draw_text_centered(renderer, font, history_array[i].task_name, task_rect, white);
                }
            }
        }
        current_x += block_w;
        time_counter += history_array[i].remaining_running_time;
    }

    // [기존 유지] 마지막 시간 눈금
    if (current_x >= start_x_position && current_x <= end_x_position) {
        char final_buf[16];
        snprintf(final_buf, sizeof(final_buf), "%d", time_counter);
        SDL_Rect final_rect = { current_x - 15, base_y_position + 45, 30, 20 };
        SDL_Color f_gray = {130, 130, 140, 255};
        draw_text_centered(renderer, font, final_buf, final_rect, f_gray);
    }
}

// 4. UI 상태 초기화 (기본값)
void initialize_ui_state(ui_scroll_state_t* state) {
    state->timeline_scroll_x = 0;
    state->stack_scroll_y = 0;
    state->queue_scroll_x = 0;
    state->queue_scroll_y = 0;
    state->scroll_speed = 30;
}

// 5. 이벤트 핸들러 (기존 유지)
void handle_user_interface_events(SDL_Event* event, ui_scroll_state_t* state, int* running) {
    if (event->type == SDL_QUIT) *running = 0;
    const Uint8* k_state = SDL_GetKeyboardState(NULL);
    int shift = (k_state[SDL_SCANCODE_LSHIFT] || k_state[SDL_SCANCODE_RSHIFT]);

    if (event->type == SDL_KEYDOWN) {
        SDL_Keycode key = event->key.keysym.sym;
        if (key == SDLK_w) state->stack_scroll_y += state->scroll_speed;
        else if (key == SDLK_s) state->stack_scroll_y -= state->scroll_speed;
        else if (key == SDLK_LEFT) {
            if (shift) state->queue_scroll_x -= state->scroll_speed;
            else state->timeline_scroll_x -= state->scroll_speed;
        }
        else if (key == SDLK_RIGHT) {
            if (shift) state->queue_scroll_x += state->scroll_speed;
            else state->timeline_scroll_x += state->scroll_speed;
        }
        else if (key == SDLK_UP && shift) state->queue_scroll_y -= state->scroll_speed;
        else if (key == SDLK_DOWN && shift) state->queue_scroll_y += state->scroll_speed;
        else if (key == SDLK_HOME) initialize_ui_state(state);
        else if (key == SDLK_ESCAPE) *running = 0;
    }
}