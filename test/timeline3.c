#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

// --- 창 크기를 데이터에 맞춰 넉넉하게 조정 ---
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 650
#define VISIBLE_X     100   // 좌측 P0~P4 라벨 공간 확보
#define TICK_W        25    // 블록이 너무 길어지지 않게 너비 조절
#define ROW_SPACING   90    // 층 간격
#define BASE_Y        500   // 가장 하단(P0) 위치

typedef struct _interrupt {
    char irq_case[100];
    int priority;
    int running_time;
} interrupt_t;

// --- 테스트 데이터 ---
interrupt_t g_history[] = {
    {"Main", 0, 5}, {"UART", 1, 4}, {"External", 3, 6}, 
    {"UART", 1, 2}, {"Main", 0, 4}, {"Timer", 4, 5},
    {"DMA", 2, 7}, {"External", 3, 3}, {"Main", 0, 5}
};
int g_block_count = 9;

void draw_text(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color) {
    if (!font || !text[0]) return;
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect tgt = { box.x + (box.w - surf->w)/2, box.y + (box.h - surf->h)/2, surf->w, surf->h };
    SDL_RenderCopy(ren, tex, NULL, &tgt);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void draw_fixed_timeline(SDL_Renderer *ren, TTF_Font *font) {
    int cur_x = VISIBLE_X;
    int total_ticks = 0;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray = {150, 150, 150, 255};

    for (int i = 0; i < 5; i++) {
        int line_y = BASE_Y - (i * ROW_SPACING);
        // 가이드 라인
        SDL_SetRenderDrawColor(ren, 60, 60, 70, 255);
        SDL_RenderDrawLine(ren, VISIBLE_X, line_y + 25, SCREEN_WIDTH - 50, line_y + 25);
        // 라벨 (P0~P4)
        char p_buf[10]; sprintf(p_buf, "P%d", i);
        SDL_Rect p_rect = { VISIBLE_X - 70, line_y, 50, 50 };
        draw_text(ren, font, p_buf, p_rect, gray);
    }

    for (int i = 0; i < g_block_count; i++) {
        int bw = g_history[i].running_time * TICK_W;
        int by = BASE_Y - (g_history[i].priority * ROW_SPACING);
        SDL_Rect r = { cur_x, by, bw - 2, 50 };

        // 색상 (이미지와 비슷하게 세팅)
        if (g_history[i].priority == 0) SDL_SetRenderDrawColor(ren, 75, 120, 185, 255);
        else {
            int r_val = 130 + (g_history[i].priority * 25);
            SDL_SetRenderDrawColor(ren, r_val, 85, 85, 255);
        }

        SDL_RenderFillRect(ren, &r);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &r);
        draw_text(ren, font, g_history[i].irq_case, r, white);

        // 하단 시간 축
        SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
        SDL_RenderDrawLine(ren, cur_x, BASE_Y + 70, cur_x, BASE_Y + 80);
        char t_buf[10]; sprintf(t_buf, "%d", total_ticks);
        SDL_Rect t_box = { cur_x - 15, BASE_Y + 85, 30, 20 };
        draw_text(ren, font, t_buf, t_box, gray);

        cur_x += bw;
        total_ticks += g_history[i].running_time;
    }

    // 마감 선
    SDL_RenderDrawLine(ren, cur_x, BASE_Y + 70, cur_x, BASE_Y + 80);
    char f_buf[10]; sprintf(f_buf, "%d", total_ticks);
    SDL_Rect f_box = { cur_x - 15, BASE_Y + 85, 30, 20 };
    draw_text(ren, font, f_buf, f_box, gray);
    SDL_RenderDrawLine(ren, VISIBLE_X, BASE_Y + 70, cur_x, BASE_Y + 70);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *win = SDL_CreateWindow("Interrupt Priority Timeline", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 13);

    int running = 1; SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;
        SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
        SDL_RenderClear(ren);
        draw_fixed_timeline(ren, font);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    TTF_CloseFont(font); TTF_Quit(); SDL_Quit();
    return 0;
}