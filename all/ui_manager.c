#include "ui_manager.h"

void init_ui_state(ui_state_t *state) {
    state->timeline_scroll_x = 0;
    state->stack_scroll_y = 0;
    state->queue_scroll_x = 0;
    state->queue_scroll_y = 0; // 초기화
    state->scroll_speed = 40;  // 한 번 누를 때 이동할 픽셀 수
}

void handle_ui_events(SDL_Event *e, ui_state_t *state, int *running) {
    if (e->type == SDL_QUIT) {
        *running = 0;
    }
    
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            // 1. 타임라인 스크롤 (방향키 좌우)
            case SDLK_LEFT:
                state->timeline_scroll_x -= state->scroll_speed;
                break;
            case SDLK_RIGHT:
                state->timeline_scroll_x += state->scroll_speed;
                break;

            // 2. 스택 스크롤 (상하 방향키 혹은 취향껏 변경 가능, 여기선 W/S와 공유)
            // 3. 큐 스크롤 (W, A, S, D)
            case SDLK_w:
                state->queue_scroll_y -= state->scroll_speed;
                state->stack_scroll_y -= state->scroll_speed; // 스택도 같이 이동
                break;
            case SDLK_s:
                state->queue_scroll_y += state->scroll_speed;
                state->stack_scroll_y += state->scroll_speed; // 스택도 같이 이동
                break;
            case SDLK_a:
                state->queue_scroll_x -= state->scroll_speed;
                break;
            case SDLK_d:
                state->queue_scroll_x += state->scroll_speed;
                break;

            // 리셋 키 (Home)
            case SDLK_HOME:
                init_ui_state(state);
                break;
        }

        // 음수 값 방지 (스크롤이 너무 뒤로 가지 않게)
        if (state->timeline_scroll_x < 0) state->timeline_scroll_x = 0;
    }
}