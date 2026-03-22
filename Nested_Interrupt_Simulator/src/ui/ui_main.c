#include "ui_main.h"
#include <stdio.h>

void initialize_ui_state(ui_scroll_state_t* state_instance) {
    state_instance->timeline_scroll_x = 0;
    state_instance->stack_scroll_y = 0;
    state_instance->queue_scroll_x = 0;
    state_instance->queue_scroll_y = 0;
    state_instance->scroll_speed = 30;
}

void handle_user_interface_events(SDL_Event* sdl_event, ui_scroll_state_t* state_instance, int* application_running) {
    if (sdl_event->type == SDL_QUIT) {
        *application_running = 0;
    }

    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    int is_shift_pressed = 0;
    
    if (keyboard_state[SDL_SCANCODE_LSHIFT] || keyboard_state[SDL_SCANCODE_RSHIFT]) {
        is_shift_pressed = 1;
    }

    if (sdl_event->type == SDL_KEYDOWN) {
        SDL_Keycode pressed_key = sdl_event->key.keysym.sym;
        
        if (pressed_key == SDLK_w) {
            state_instance->stack_scroll_y += state_instance->scroll_speed;
        }
        else if (pressed_key == SDLK_s) {
            state_instance->stack_scroll_y -= state_instance->scroll_speed;
        }
        else if (pressed_key == SDLK_LEFT) {
            if (is_shift_pressed == 1) {
                state_instance->queue_scroll_x -= state_instance->scroll_speed;
            }
            else {
                state_instance->timeline_scroll_x -= state_instance->scroll_speed;
            }
        }
        else if (pressed_key == SDLK_RIGHT) {
            if (is_shift_pressed == 1) {
                state_instance->queue_scroll_x += state_instance->scroll_speed;
            }
            else {
                state_instance->timeline_scroll_x += state_instance->scroll_speed;
            }
        }
        else if (pressed_key == SDLK_UP) {
            if (is_shift_pressed == 1) {
                state_instance->queue_scroll_y -= state_instance->scroll_speed;
            }
        }
        else if (pressed_key == SDLK_DOWN) {
            if (is_shift_pressed == 1) {
                state_instance->queue_scroll_y += state_instance->scroll_speed;
            }
        }
        else if (pressed_key == SDLK_HOME) {
            initialize_ui_state(state_instance);
        }
        else if (pressed_key == SDLK_ESCAPE) {
            *application_running = 0;
        }
    }
}

void set_priority_color(SDL_Renderer* renderer, int priority_level) {
    if (priority_level == 0) {
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
    }
    else if (priority_level == 1) {
        SDL_SetRenderDrawColor(renderer, 200, 100, 50, 255);
    }
    else if (priority_level == 2) {
        SDL_SetRenderDrawColor(renderer, 200, 180, 50, 255);
    }
    else if (priority_level == 3) {
        SDL_SetRenderDrawColor(renderer, 50, 180, 50, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 50, 100, 200, 255);
    }
}

void draw_text_centered(SDL_Renderer* renderer, TTF_Font* font, const char* text_string, SDL_Rect bounding_box, SDL_Color text_color) {
    if (text_string == NULL || text_string[0] == '\0') {
        return;
    }
    
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text_string, text_color);
    if (text_surface == NULL) {
        return;
    }
    
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    
    int center_x = bounding_box.x + (bounding_box.w - text_surface->w) / 2;
    int center_y = bounding_box.y + (bounding_box.h - text_surface->h) / 2;
    
    SDL_Rect target_rectangle = { center_x, center_y, text_surface->w, text_surface->h };
    
    SDL_RenderCopy(renderer, text_texture, NULL, &target_rectangle);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void render_timeline(SDL_Renderer* renderer, TTF_Font* font, interrupt_task_t* history_array, int total_count, int offset_x) {
    int start_x_position = 80;   
    int screen_width = 1600; 
    int end_x_position = 1550;    
    int base_y_position = 380;   
    int tick_width = 40;    
    int row_height = 60;     

    int target_x_position = (int)(screen_width * 0.9); 
    int total_data_width = 0;
    
    for (int index = 0; index < total_count; index++) {
        total_data_width += (history_array[index].remaining_running_time * tick_width);
    }
    
    int timeline_base_x_position = target_x_position - total_data_width - offset_x;

    for (int level = 0; level <= 4; level++) {
        int y_position = base_y_position - (level * row_height);
        
        SDL_SetRenderDrawColor(renderer, 50, 50, 55, 255);
        SDL_RenderDrawLine(renderer, start_x_position, y_position + 25, end_x_position, y_position + 25);
        
        char priority_label[10];
        snprintf(priority_label, sizeof(priority_label), "P%d", level);
        
        SDL_Rect label_rectangle = { start_x_position - 65, y_position, 50, 50 };
        SDL_Color gray_color = {150, 150, 150, 255};
        
        draw_text_centered(renderer, font, priority_label, label_rectangle, gray_color);
    }

    int current_x_position = timeline_base_x_position;
    int time_counter = 0; 

    for (int index = 0; index < total_count; index++) {
        int block_width = history_array[index].remaining_running_time * tick_width;
        int block_y_position = base_y_position - (history_array[index].task_priority * row_height);

        if (current_x_position >= start_x_position && current_x_position < end_x_position) {
            char time_buffer[16];
            snprintf(time_buffer, sizeof(time_buffer), "%d", time_counter);
            
            SDL_Rect time_rectangle = { current_x_position - 15, base_y_position + 45, 30, 20 };
            SDL_Color light_gray_color = {130, 130, 140, 255};
            
            draw_text_centered(renderer, font, time_buffer, time_rectangle, light_gray_color);
            
            SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
            SDL_RenderDrawLine(renderer, current_x_position, base_y_position + 30, current_x_position, base_y_position + 40);
        }

        if (current_x_position + block_width > start_x_position && current_x_position < end_x_position) {
            int draw_x_position;
            if (current_x_position < start_x_position) {
                draw_x_position = start_x_position;
            } else {
                draw_x_position = current_x_position;
            }
            
            int draw_width;
            if (current_x_position < start_x_position) {
                draw_width = block_width - (start_x_position - current_x_position);
            } else {
                draw_width = block_width;
            }
            
            if (draw_x_position + draw_width > end_x_position) {
                draw_width = end_x_position - draw_x_position;
            }

            SDL_Rect task_rectangle = { draw_x_position, block_y_position, draw_width - 2, 50 };
            
            set_priority_color(renderer, history_array[index].task_priority);
            SDL_RenderFillRect(renderer, &task_rectangle);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &task_rectangle);

            SDL_Color white_color = {255, 255, 255, 255};

            if (draw_width > 70) {
                char first_line_buffer[128];
                char second_line_buffer[64];
                
                snprintf(first_line_buffer, sizeof(first_line_buffer), "%s", history_array[index].task_name);
                snprintf(second_line_buffer, sizeof(second_line_buffer), "(%d ms)", history_array[index].remaining_running_time);

                SDL_Rect top_text_rectangle = { task_rectangle.x, task_rectangle.y + 5, task_rectangle.w, 20 };
                SDL_Rect bottom_text_rectangle = { task_rectangle.x, task_rectangle.y + 25, task_rectangle.w, 20 };
                
                draw_text_centered(renderer, font, first_line_buffer, top_text_rectangle, white_color);
                draw_text_centered(renderer, font, second_line_buffer, bottom_text_rectangle, white_color);
            } 
            else if (draw_width > 30) {
                draw_text_centered(renderer, font, history_array[index].task_name, task_rectangle, white_color);
            }
        }
        
        current_x_position += block_width;
        time_counter += history_array[index].remaining_running_time;
    }

    if (current_x_position >= start_x_position && current_x_position < end_x_position) {
        char final_time_buffer[16];
        snprintf(final_time_buffer, sizeof(final_time_buffer), "%d", time_counter);
        
        SDL_Rect final_time_rectangle = { current_x_position - 15, base_y_position + 45, 30, 20 };
        SDL_Color final_gray_color = {130, 130, 140, 255};
        
        draw_text_centered(renderer, font, final_time_buffer, final_time_rectangle, final_gray_color);
    }
}