#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SDL2/SDL.h>

// UI 상태를 관리하는 구조체
typedef struct {
    int timeline_scroll_x; // 타임라인 좌우 스크롤
    int stack_scroll_y;    // 스택 상하 스크롤
    int queue_scroll_x;    // 큐 좌우 스크롤
    int queue_scroll_y;    // [추가] 큐 상하 스크롤
    int scroll_speed;      // 스크롤 속도
} ui_state_t;

// 함수 프로토타입
void init_ui_state(ui_state_t *state);
void handle_ui_events(SDL_Event *e, ui_state_t *state, int *running);

#endif