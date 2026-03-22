#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAX_STACK_SIZE 10
#define MAX_QUEUE_SIZE 10

typedef struct _interrupt {
    char irq_case[100];
    int priority;
    int running_time;
} interrupt_t;

// main.c에서 구현할 공통 렌더링 도우미
void draw_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color);

#endif