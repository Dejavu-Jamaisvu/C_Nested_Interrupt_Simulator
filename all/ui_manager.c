#include "ui_manager.h"

void init_ui_state(ui_state_t *state) {
    state->timeline_scroll_x = 0; 
    state->stack_scroll_y = 0;      
    state->queue_scroll_x = 0;
    state->queue_scroll_y = 0;      
    state->scroll_speed = 30; 
}

void handle_ui_events(SDL_Event *e, ui_state_t *state, int *running) {
    if (e->type == SDL_QUIT) *running = 0;
    
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    int isShift = keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT];

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_w: 
                // W: 위쪽 블록들을 확인하기 위해 전체를 아래로 내림 (+)
                state->stack_scroll_y += state->scroll_speed; 
                break;
            case SDLK_s: 
                // S: 아래쪽 블록들을 확인하기 위해 전체를 위로 올림 (-)
                state->stack_scroll_y -= state->scroll_speed; 
                break;
            case SDLK_LEFT:
                if (isShift) state->queue_scroll_x -= state->scroll_speed;
                else         state->timeline_scroll_x -= state->scroll_speed;
                break;
            case SDLK_RIGHT:
                if (isShift) state->queue_scroll_x += state->scroll_speed;
                else         state->timeline_scroll_x += state->scroll_speed;
                break;
            case SDLK_UP:
                if (isShift) state->queue_scroll_y -= state->scroll_speed;
                break;
            case SDLK_DOWN:
                if (isShift) state->queue_scroll_y += state->scroll_speed;
                break;
            case SDLK_HOME:
                init_ui_state(state);
                break;
            case SDLK_ESCAPE:
                *running = 0;
                break;
        }
    }
}