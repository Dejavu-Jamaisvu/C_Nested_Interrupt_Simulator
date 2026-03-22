#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "stack.h"
#include "queue.h"
#include "scheduler.h"

#include "ui_main.h"
#include "ui_stack.h"
#include "ui_queue.h"

extern resource_status_t global_cpu_status;
extern interrupt_task_t task_history_array[MAXIMUM_HISTORY_SIZE];
extern int history_count;
extern int current_task_duration;

typedef struct _scenario_event {
    int arrival_time;
    interrupt_task_t interrupt_task;
} scenario_event_t;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    if (TTF_Init() < 0) {
        return -1;
    }

    SDL_Window* application_window = SDL_CreateWindow("RTOS Interrupt Monitor - Interactive Mode", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    SDL_Renderer* graphic_renderer = SDL_CreateRenderer(application_window, -1, SDL_RENDERER_ACCELERATED);
    
    TTF_Font* default_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    
    if (default_font == NULL) {
        default_font = TTF_OpenFont("/usr/share/fonts/google-noto-vf/NotoSans[wght].ttf", 14);
    }
    
    if (default_font == NULL) {
        // 둘 다 없다면 에러 출력 후 종료
        printf("Font Load Error: Could not find fonts\n");
        return -1;
    }
    // =========================================================================

    ui_scroll_state_t user_interface_state;
    initialize_ui_state(&user_interface_state);
    
    nested_stack_t system_stack;
    priority_queue_t system_queue;
    
    initialize_stack(&system_stack);
    initialize_queue(&system_queue);
    initialize_scheduler();

    scenario_event_t test_scenarios[] = {
        {0,  {"Init_Task", 4, 8}}, 
        {4,  {"Net_RX", 2, 6}}, 
        {15, {"Sensor", 3, 4}}
    };
    
    int total_scenario_count = sizeof(test_scenarios) / sizeof(test_scenarios[0]);
    int current_scenario_index = 0;

    int application_running = 1;
    SDL_Event sdl_event;
    Uint32 last_tick_timestamp = SDL_GetTicks();
    int global_system_time = 0;

    while (application_running == 1) {
        
        while (SDL_PollEvent(&sdl_event)) {
            handle_user_interface_events(&sdl_event, &user_interface_state, &application_running);
            
            if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.repeat == 0) {
                SDL_Keycode pressed_key = sdl_event.key.keysym.sym;
                
                if (pressed_key == SDLK_1) {
                    interrupt_task_t manual_task = {"Manual_P0", 0, 5};
                    handle_incoming_interrupt(manual_task, &system_stack, &system_queue);
                }
                else if (pressed_key == SDLK_2) {
                    interrupt_task_t manual_task = {"Manual_P1", 1, 5};
                    handle_incoming_interrupt(manual_task, &system_stack, &system_queue);
                }
                else if (pressed_key == SDLK_3) {
                    interrupt_task_t manual_task = {"Manual_P2", 2, 5};
                    handle_incoming_interrupt(manual_task, &system_stack, &system_queue);
                }
                else if (pressed_key == SDLK_4) {
                    interrupt_task_t manual_task = {"Manual_P3", 3, 5};
                    handle_incoming_interrupt(manual_task, &system_stack, &system_queue);
                }
                else if (pressed_key == SDLK_5) {
                    interrupt_task_t manual_task = {"Manual_P4", 4, 5};
                    handle_incoming_interrupt(manual_task, &system_stack, &system_queue);
                }
            }
        }

        Uint32 current_timestamp = SDL_GetTicks();
        if (current_timestamp - last_tick_timestamp >= 1000) {
            
            while (current_scenario_index < total_scenario_count) {
                if (global_system_time == test_scenarios[current_scenario_index].arrival_time) {
                    handle_incoming_interrupt(test_scenarios[current_scenario_index].interrupt_task, &system_stack, &system_queue);
                    current_scenario_index++;
                }
                else {
                    break;
                }
            }

            execute_system_tick(&system_stack, &system_queue); 
            global_system_time++; 
            last_tick_timestamp = current_timestamp;
        }

        SDL_SetRenderDrawColor(graphic_renderer, 15, 15, 20, 255);
        SDL_RenderClear(graphic_renderer);

        interrupt_task_t render_history_array[MAXIMUM_HISTORY_SIZE + 1];
        memcpy(render_history_array, task_history_array, sizeof(interrupt_task_t) * history_count);
        int current_render_count = history_count;

        if (global_cpu_status.is_cpu_idle == false) {
            if (current_task_duration > 0) {
                render_history_array[current_render_count] = global_cpu_status.current_task;
                render_history_array[current_render_count].remaining_running_time = current_task_duration;
                current_render_count++;
            }
        }

        render_timeline(graphic_renderer, default_font, render_history_array, current_render_count, user_interface_state.timeline_scroll_x);
        render_nested_stack(graphic_renderer, default_font, &system_stack, user_interface_state.stack_scroll_y);
        render_priority_queue(graphic_renderer, default_font, &system_queue, user_interface_state.queue_scroll_x, user_interface_state.queue_scroll_y);

        char status_message_buffer[256];
        if (global_cpu_status.is_cpu_idle == true) {
            snprintf(status_message_buffer, sizeof(status_message_buffer), "Global Time: %d sec | CPU: IDLE | Press 1~5 to trigger P0~P4 Interrupts!", global_system_time);
        }
        else {
            snprintf(status_message_buffer, sizeof(status_message_buffer), "Global Time: %d sec | CPU: [%s] (Priority: %d, %d ms left) | Press 1~5 to trigger P0~P4 Interrupts!", 
                     global_system_time, global_cpu_status.current_task.task_name, global_cpu_status.current_task.task_priority, global_cpu_status.current_task.remaining_running_time);
        }
        
        SDL_Rect status_bar_rectangle = {0, 10, 1600, 30};
        SDL_Color light_green_color = {200, 255, 200, 255};
        draw_text_centered(graphic_renderer, default_font, status_message_buffer, status_bar_rectangle, light_green_color);

        SDL_RenderPresent(graphic_renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(default_font);
    TTF_Quit();
    SDL_DestroyRenderer(graphic_renderer);
    SDL_DestroyWindow(application_window);
    SDL_Quit();
    
    return 0;
}