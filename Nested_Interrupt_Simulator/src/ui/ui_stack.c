// [ui_stack.c]
#include "ui_stack.h"
#include "ui_main.h"
#include <stdio.h>
#include <string.h>

void render_nested_stack(SDL_Renderer* renderer, TTF_Font* font, nested_stack_t* stack_instance, int offset_y) {
    SDL_Rect background_area = { 50, 450, 400, 400 }; 
    SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
    SDL_RenderFillRect(renderer, &background_area);
    
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderDrawRect(renderer, &background_area);

    SDL_Rect title_box = { background_area.x, background_area.y + 10, background_area.w, 30 };
    SDL_Color yellow_color = {255, 255, 100, 255};
    draw_text_centered(renderer, font, "NESTED STACK (LIFO)", title_box, yellow_color);

    if (stack_instance->current_data_count <= 0) {
        return;
    }

    SDL_Rect clipping_area = { background_area.x + 5, background_area.y + 45, background_area.w - 10, background_area.h - 55 };
    SDL_RenderSetClipRect(renderer, &clipping_area);

    int block_width = 340;
    int block_height = 45;
    int block_gap = 10;
    int step_size = block_height + block_gap;
    int top_y_position = background_area.y + 55; 

    for (int index = 0; index < stack_instance->current_data_count; index++) {
        int reversed_index = (stack_instance->current_data_count - 1) - index;
        int current_y_position = top_y_position + (index * step_size) + offset_y;

        if (current_y_position > background_area.y - 50 && current_y_position < background_area.y + background_area.h + 50) {
            SDL_Rect task_block = { background_area.x + (background_area.w - block_width) / 2, current_y_position, block_width, block_height };
            
            set_priority_color(renderer, stack_instance->stack_array[reversed_index].task_priority);
            SDL_RenderFillRect(renderer, &task_block);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &task_block);

            char display_buffer[256];
            snprintf(display_buffer, sizeof(display_buffer), "[Priority %d] %s (%d ms)", 
                    stack_instance->stack_array[reversed_index].task_priority, 
                    stack_instance->stack_array[reversed_index].task_name, 
                    stack_instance->stack_array[reversed_index].remaining_running_time);

            if (reversed_index == stack_instance->current_data_count - 1) {
                snprintf(display_buffer + strlen(display_buffer), sizeof(display_buffer) - strlen(display_buffer), " [Top]");
            }
            else if (reversed_index == 0) {
                snprintf(display_buffer + strlen(display_buffer), sizeof(display_buffer) - strlen(display_buffer), " [Bottom]");
            }

            SDL_Color white_color = {255, 255, 255, 255};
            draw_text_centered(renderer, font, display_buffer, task_block, white_color);
        }
    }
    
    SDL_RenderSetClipRect(renderer, NULL);
}