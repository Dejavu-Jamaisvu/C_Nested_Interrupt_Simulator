// [ui_queue.c]
#include "ui_queue.h"
#include "ui_main.h"
#include <stdio.h>

void render_priority_queue(SDL_Renderer* renderer, TTF_Font* font, priority_queue_t* queue_instance, int offset_x, int offset_y) {
    SDL_Rect background_area = { 480, 450, 1050, 400 }; 
    
    SDL_SetRenderDrawColor(renderer, 35, 35, 40, 255);
    SDL_RenderFillRect(renderer, &background_area);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
    SDL_RenderDrawRect(renderer, &background_area);

    SDL_Rect title_box = { 480, 460, 1050, 30 };
    SDL_Color cyan_color = {100, 255, 255, 255};
    draw_text_centered(renderer, font, "PRIORITY QUEUE (MIN-HEAP TREE)", title_box, cyan_color);

    if (queue_instance->current_size <= 0) {
        return;
    }

    SDL_Rect clipping_area = { background_area.x + 5, background_area.y + 45, background_area.w - 10, background_area.h - 55 };
    SDL_RenderSetClipRect(renderer, &clipping_area);

    int node_x_array[MAXIMUM_QUEUE_SIZE];
    int node_y_array[MAXIMUM_QUEUE_SIZE];
    
    int minimum_node_gap = 140; 
    int root_x_position = (background_area.x + background_area.w / 2) - offset_x;
    int root_y_position = (background_area.y + 80) - offset_y; 

    for (int index = 0; index < queue_instance->current_size; index++) {
        int current_level = 0;
        int temporary_value = index + 1;
        
        while (temporary_value >>= 1) {
            current_level++;
        }
        
        int position_in_level = index - ((1 << current_level) - 1);
        int nodes_in_current_level = (1 << current_level);
        int maximum_tree_level = 3; 
        
        int node_spacing = minimum_node_gap * (1 << (maximum_tree_level - current_level));
        float start_x_position = root_x_position - ((nodes_in_current_level - 1) * node_spacing) / 2.0f;
        
        node_x_array[index] = (int)(start_x_position + (position_in_level * node_spacing));
        node_y_array[index] = root_y_position + (current_level * 100);

        if (index > 0) {
            int parent_index = (index - 1) / 2;
            SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
            SDL_RenderDrawLine(renderer, node_x_array[parent_index], node_y_array[parent_index], node_x_array[index], node_y_array[index]);
        }
    }

    for (int index = 0; index < queue_instance->current_size; index++) {
        SDL_Rect node_rectangle = { node_x_array[index] - 55, node_y_array[index] - 25, 110, 50 };

        set_priority_color(renderer, queue_instance->queue_array[index].task_priority);
        SDL_RenderFillRect(renderer, &node_rectangle);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &node_rectangle);

        char top_line_buffer[128];
        char bottom_line_buffer[64];
        
        snprintf(top_line_buffer, sizeof(top_line_buffer), "P%d:%s", queue_instance->queue_array[index].task_priority, queue_instance->queue_array[index].task_name);
        snprintf(bottom_line_buffer, sizeof(bottom_line_buffer), "(%d ms)", queue_instance->queue_array[index].remaining_running_time);

        SDL_Rect top_text_rectangle = { node_rectangle.x, node_rectangle.y + 5, node_rectangle.w, 20 };
        SDL_Rect bottom_text_rectangle = { node_rectangle.x, node_rectangle.y + 25, node_rectangle.w, 20 };

        SDL_Color white_color = {255, 255, 255, 255};
        draw_text_centered(renderer, font, top_line_buffer, top_text_rectangle, white_color);
        draw_text_centered(renderer, font, bottom_line_buffer, bottom_text_rectangle, white_color);
    }

    SDL_RenderSetClipRect(renderer, NULL);
}