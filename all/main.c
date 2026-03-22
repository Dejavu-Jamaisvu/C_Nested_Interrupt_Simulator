// main.c
#include "stack.h"
#include "queue.h"
#include "timeline.h"
#include <stdio.h>

// 공통 텍스트 렌더링 (main.c에 위치)
void draw_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color) {
    if (!text || text[0] == '\0') return;
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect tgt = { box.x + (box.w - surf->w)/2, box.y + (box.h - surf->h)/2, surf->w, surf->h };
    SDL_RenderCopy(ren, tex, NULL, &tgt);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *win = SDL_CreateWindow("Stress Test - Overload", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);

    // 1. 타임라인 폭주 (총 실행 시간이 화면 가로폭 1200px를 넘는지 확인)
    // 4 + 3 + 5 + 2 + 4 + 3 + 6 + 2 + 5 + 4 = 38 ticks (38 * 35px = 1330px -> 화면 밖으로 나감!)
    interrupt_t g_history[] = {
        {"Main",0,4}, {"UART",1,3}, {"DMA",2,5}, {"Ext",3,2}, {"Timer",4,4},
        {"I2C",1,3}, {"USB",2,6}, {"CAN",3,2}, {"SPI",1,5}, {"ADC",4,4}
    };
    int g_hist_count = 10;

    // 2. 스택 폭주 (MAX_STACK_SIZE인 10개 꽉 채우기)
    // 박스 높이를 뚫고 올라가는지 확인용
    nested_stack_t g_stack = {
        .data_count = 10,
        .stack = {
            {"Main",0,4}, {"S1",1,1}, {"S2",2,1}, {"S3",3,1}, {"S4",4,1},
            {"S5",1,1}, {"S6(Over)",2,1}, {"S7",3,1}, {"S8",4,1}, {"S9(Top)",4,1}
        }
    };

    // 3. 큐 폭주 (트리 노드 10개)
    // 4층(Level 3)까지 내려가면서 노드끼리 겹치는지 확인용
    priority_queue_t g_queue = {
        .size = 10,
        .data = {
            {"P4_T0",4,0}, {"P3_T1",3,0}, {"P2_T2",2,0}, {"P1_T3",1,0}, 
            {"P1_T4",1,0}, {"P0_T5",0,0}, {"P2_T6",2,0}, {"P3_T7",3,0},
            {"P4_T8",4,0}, {"P0_T9",0,0}
        }
    };

    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;

        SDL_SetRenderDrawColor(ren, 15, 15, 20, 255);
        SDL_RenderClear(ren);

        render_timeline(ren, font, g_history, g_hist_count);
        render_stack(ren, font, &g_stack);
        render_queue(ren, font, &g_queue);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}