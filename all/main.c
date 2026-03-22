#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "ui_manager.h"
#include "stack.h"
#include "queue.h"
#include "timeline.h"
#include "common.h"
#include <stdio.h>

// 공통 텍스트 렌더링 (중앙 정렬)
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
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    if (TTF_Init() < 0) return -1;

    SDL_Window *win = SDL_CreateWindow("Interrupt Monitor - Stress Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    
    // 폰트 경로 (시스템 환경에 맞게 조정 필요)
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!font) {
        printf("Font Load Error: %s\n", TTF_GetError());
        return -1;
    }

    ui_state_t ui;
    init_ui_state(&ui);

    // 1. 타임라인 데이터 (이미 다양함)
    interrupt_t g_history[] = {
        {"Main",0,4}, {"UART",1,3}, {"DMA",2,5}, {"Ext",3,2}, {"Timer",4,4},
        {"I2C",1,3}, {"USB",2,6}, {"CAN",3,2}, {"SPI",1,5}, {"ADC",4,4}
    };
    int g_hist_count = 10;

    // 2. 스택 데이터 (이름 다양화 및 (Top) 제거)
    // 실제 중첩(Nesting) 상황을 가정하여 다양한 이름을 넣었습니다.
    nested_stack_t g_stack = {
        .data_count = 10,
        .stack = {
            {"Main", 0, 10}, {"UART", 1, 8},  {"DMA", 2, 6},   {"I2C", 1, 5},   {"USB", 2, 4},
            {"Ext", 3, 3},  {"CAN", 3, 2},   {"Timer", 4, 1}, {"SPI", 1, 1},   {"ADC", 4, 1}
        }
    };

    // 3. 큐 데이터 (대기 중인 인터럽트들 다양화)
    // 트리 구조에서 보기 좋게 이름을 UART, DMA 등으로 변경했습니다.
    priority_queue_t g_queue = {
        .size = 10,
        .data = {
            {"ADC_0", 4, 10}, {"Timer_1", 4, 8}, {"CAN_2", 3, 5}, {"Ext_3", 3, 4}, 
            {"USB_4", 2, 7},  {"DMA_5", 2, 3},   {"I2C_6", 1, 6}, {"UART_7", 1, 2},
            {"SPI_8", 1, 4},  {"Main_9", 0, 1}
        }
    };

    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            handle_ui_events(&e, &ui, &running);
        }

        // 배경색
        SDL_SetRenderDrawColor(ren, 15, 15, 20, 255);
        SDL_RenderClear(ren);

        // --- 렌더링 호출 (스크롤 값 전달) ---
        
        // 타임라인 (좌우 스크롤)
        render_timeline(ren, font, g_history, g_hist_count, ui.timeline_scroll_x);

        // 스택 (W/S 상하 스크롤)
        render_stack(ren, font, &g_stack, ui.stack_scroll_y);

        // 큐 (Shift+방향키 상하좌우 스크롤)
        render_queue(ren, font, &g_queue, ui.queue_scroll_x, ui.queue_scroll_y);

        SDL_RenderPresent(ren);
        SDL_Delay(16); 
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return 0;
}