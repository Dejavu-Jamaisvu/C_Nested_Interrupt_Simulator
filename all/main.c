#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "ui_manager.h"
#include "stack.h"
#include "queue.h"
#include "timeline.h"
#include "common.h"
#include <stdio.h>

// 공통 텍스트 렌더링 (함수명 수정 완료: TTF_RenderText_Blended)
void draw_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color) {
    if (!text || text[0] == '\0') return;
    
    // 이 부분이 수정되었습니다.
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
    
    // 시스템 폰트 경로 (환경에 따라 다를 수 있으니 주의)
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!font) {
        printf("Font Load Error: %s\n", TTF_GetError());
        return -1;
    }

    // 1. UI 상태 초기화 (모든 스크롤 값 0)
    ui_state_t ui;
    init_ui_state(&ui);

    // 2. 타임라인 데이터 (스크롤 테스트용)
    interrupt_t g_history[] = {
        {"Main",0,4}, {"UART",1,3}, {"DMA",2,5}, {"Ext",3,2}, {"Timer",4,4},
        {"I2C",1,3}, {"USB",2,6}, {"CAN",3,2}, {"SPI",1,5}, {"ADC",4,4}
    };
    int g_hist_count = 10;

    // 3. 스택 데이터 (스크롤 테스트를 위해 10개 유지)
    nested_stack_t g_stack = {
        .data_count = 10,
        .stack = {
            {"Main",0,4}, {"S1",1,1}, {"S2",2,1}, {"S3",3,1}, {"S4",4,1},
            {"S5",1,1}, {"S6(Over)",2,1}, {"S7",3,1}, {"S8",4,1}, {"S9(Top)",4,1}
        }
    };

    // 4. 큐 데이터 (트리 노드 10개)
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
        // [EVENT] UI 매니저에게 이벤트 처리를 맡깁니다.
        while (SDL_PollEvent(&e)) {
            handle_ui_events(&e, &ui, &running);
        }

        // 배경색 설정
        SDL_SetRenderDrawColor(ren, 15, 15, 20, 255);
        SDL_RenderClear(ren);

        // [RENDER] 각 모듈에 업데이트된 스크롤(offset) 값 전달
        
        // 타임라인: X축 스크롤 (방향키 좌우)
        render_timeline(ren, font, g_history, g_hist_count, ui.timeline_scroll_x);

        // 스택: Y축 스크롤 (W, S키)
        render_stack(ren, font, &g_stack, ui.stack_scroll_y);

        // 큐: X축, Y축 모두 적용 (W, A, S, D키)
        // [수정] 인자 5개로 호출: ren, font, queue, offset_x, offset_y
        render_queue(ren, font, &g_queue, ui.queue_scroll_x, ui.queue_scroll_y);

        SDL_RenderPresent(ren);
        SDL_Delay(16); // 약 60FPS 유지
    }

    // 자원 해제
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return 0;
}