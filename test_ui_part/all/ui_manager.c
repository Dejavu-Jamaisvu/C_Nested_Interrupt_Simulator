#include "ui_manager.h"

void init_ui_state(ui_state_t *state) {
    state->timeline_scroll_x = 0; 
    state->stack_scroll_y = 0;      
    state->queue_scroll_x = 0;
    state->queue_scroll_y = 0;      
    state->scroll_speed = 30; // 뚝뚝 끊기지 않게 적당한 속도
}

void handle_ui_events(SDL_Event *e, ui_state_t *state, int *running) {
    if (e->type == SDL_QUIT) *running = 0;
    
    // Shift 키가 눌렸는지 확인 (타임라인 vs 큐 조작 구분용)
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    int isShift = keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT];

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            // --- 스택(Stack) 제어: W/S 키 ---
            case SDLK_w: 
                // 위쪽 블록 확인 (내용물을 아래로 밀기)
                state->stack_scroll_y += state->scroll_speed; 
                break;
            case SDLK_s: 
                // 아래쪽 블록 확인 (내용물을 위로 밀기)
                state->stack_scroll_y -= state->scroll_speed; 
                break;

            // --- 타임라인/큐 좌우 제어: 방향키 LEFT/RIGHT ---
            case SDLK_LEFT:
                if (isShift) state->queue_scroll_x -= state->scroll_speed;
                else         state->timeline_scroll_x -= state->scroll_speed;
                break;
            case SDLK_RIGHT:
                if (isShift) state->queue_scroll_x += state->scroll_speed;
                else         state->timeline_scroll_x += state->scroll_speed;
                break;

            // --- 큐(Queue) 상하 제어: 방향키 UP/DOWN (Shift 조합) ---
            case SDLK_UP:
                if (isShift) state->queue_scroll_y -= state->scroll_speed;
                break;
            case SDLK_DOWN:
                if (isShift) state->queue_scroll_y += state->scroll_speed;
                break;

            // --- 공통 기능 ---
            case SDLK_HOME:
                init_ui_state(state);
                break;
            case SDLK_ESCAPE:
                *running = 0;
                break;
        }
    }
}