#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "types.h"

typedef struct _ui_scroll_state {
    int timeline_scroll_x;
    int stack_scroll_y;
    int queue_scroll_x;
    int queue_scroll_y;
    int scroll_speed;
} ui_scroll_state_t;

void initialize_ui_state(ui_scroll_state_t* state_instance);
void handle_user_interface_events(SDL_Event* sdl_event, ui_scroll_state_t* state_instance, int* application_running);
void set_priority_color(SDL_Renderer* renderer, int priority_level);
void draw_text_centered(SDL_Renderer* renderer, TTF_Font* font, const char* text_string, SDL_Rect bounding_box, SDL_Color text_color);
void render_timeline(SDL_Renderer* renderer, TTF_Font* font, interrupt_task_t* history_array, int total_count, int offset_x);

#endif