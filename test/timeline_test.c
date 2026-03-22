#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 480
#define TICK_WIDTH 25      // 한 칸의 너비
#define VISIBLE_X 100      // 타임라인 시작 X
#define VISIBLE_W 1080     // 타임라인 전체 너비
#define RED_LINE_X (VISIBLE_X + VISIBLE_W - 150) // 빨간 선 고정 위치 (우측에서 150px 앞)

typedef struct {
    int task_type; // 0: Main, 1: ISR1, 2: ISR2
    int duration;
    int is_completed;
} block_t;

block_t g_history[500];
int g_block_count = 0;
int g_total_ticks = 0;

// --- [렌더링 함수: 선과 블록의 동기화] ---
void draw_synced_timeline(SDL_Renderer *ren) {
    // 1. 배경 및 틀 그리기
    SDL_Rect bg = { VISIBLE_X, 100, VISIBLE_W, 300 };
    SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
    SDL_RenderFillRect(ren, &bg);

    // 2. 스크롤 오프셋 계산 (핵심!)
    // 현재까지의 총 길이 중 빨간 선 위치를 넘어가는 만큼만 왼쪽으로 민다.
    int total_w = g_total_ticks * TICK_WIDTH;
    int offset_x = 0;
    if (total_w > (RED_LINE_X - VISIBLE_X)) {
        offset_x = total_w - (RED_LINE_X - VISIBLE_X);
    }

    // 영역 밖으로 나가는 그림은 잘라내기(Clip)
    SDL_RenderSetClipRect(ren, &bg);

    int current_x_offset = 0;
    for (int i = 0; i < g_block_count; i++) {
        // 실제 그릴 좌표 = 시작점 + 블록위치 - 스크롤양
        int x = VISIBLE_X + (current_x_offset * TICK_WIDTH) - offset_x;
        int w = g_history[i].duration * TICK_WIDTH;
        
        // 우선순위(task_type)에 따라 높이(Y) 조절
        int y = 300 - (g_history[i].task_type * 70); 

        SDL_Rect r = { x, y, w - 2, 50 }; // 블록 사이 간격 2px
        
        if (g_history[i].task_type == 0) SDL_SetRenderDrawColor(ren, 70, 130, 180, 255);
        else if (g_history[i].task_type == 1) SDL_SetRenderDrawColor(ren, 200, 150, 50, 255);
        else SDL_SetRenderDrawColor(ren, 200, 80, 80, 255);
        
        SDL_RenderFillRect(ren, &r);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &r);

        current_x_offset += g_history[i].duration;
    }

    // 3. 빨간 선 (현재 시점 가이드)
    // 데이터가 차오를 땐 따라가고, RED_LINE_X에 도달하면 고정됨
    int final_red_x = VISIBLE_X + (g_total_ticks * TICK_WIDTH) - offset_x;
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    SDL_RenderDrawLine(ren, final_red_x, bg.y, final_red_x, bg.y + bg.h);

    SDL_RenderSetClipRect(ren, NULL); // 클립 해제
}

// --- [가상 데이터 업데이트 타이머] ---
Uint32 update_tick(Uint32 interval, void *param) {
    g_total_ticks++;
    if (g_block_count == 0 || g_history[g_block_count-1].is_completed) {
        // 새 블록 생성 (랜덤 타입)
        g_history[g_block_count].task_type = rand() % 3;
        g_history[g_block_count].duration = 1;
        g_history[g_block_count].is_completed = 0;
        g_block_count++;
    } else {
        g_history[g_block_count-1].duration++;
        if (rand() % 8 == 0) g_history[g_block_count-1].is_completed = 1;
    }
    return interval;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *win = SDL_CreateWindow("Perfect Sync Timeline", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_AddTimer(200, update_tick, NULL); // 0.2초마다 1틱씩 진행

    int running = 1; SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;
        SDL_SetRenderDrawColor(ren, 20, 20, 25, 255);
        SDL_RenderClear(ren);
        draw_synced_timeline(ren);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}