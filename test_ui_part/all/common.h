#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define MAX_STACK_SIZE 10
#define MAX_QUEUE_SIZE 10

typedef struct _interrupt {
    char irq_case[100];
    int priority;
    int running_time;
} interrupt_t;

// 공통 텍스트 렌더링 함수 선언
void draw_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color);

// 우선순위별 공통 색상 지정 함수 (헤더에 직접 구현하기 위해 static inline 사용)
static inline void set_priority_color(SDL_Renderer *ren, int p) {
    if      (p == 0) SDL_SetRenderDrawColor(ren, 50, 120, 200, 255); // Blue
    else if (p == 1) SDL_SetRenderDrawColor(ren, 60, 180, 70, 255);  // Green
    else if (p == 2) SDL_SetRenderDrawColor(ren, 220, 200, 50, 255); // Yellow
    else if (p == 3) SDL_SetRenderDrawColor(ren, 240, 130, 40, 255); // Orange
    else             SDL_SetRenderDrawColor(ren, 220, 50, 50, 255);  // Red
}

#endif