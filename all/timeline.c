
// timeline.c
#include "timeline.h"

void render_timeline(SDL_Renderer *ren, TTF_Font *font, interrupt_t *history, int count) {
    int start_x = 100, base_y = 320, tick_w = 30, row_h = 60;
    
    for (int i = 0; i < 5; i++) { // P0~P4 가이드라인
        int y = base_y - (i * row_h);
        SDL_SetRenderDrawColor(ren, 60, 60, 70, 255);
        SDL_RenderDrawLine(ren, start_x, y + 20, 1200, y + 20);
    }

    int cur_x = start_x;
    for (int i = 0; i < count; i++) {
        int bw = history[i].running_time * tick_w;
        int by = base_y - (history[i].priority * row_h);
        SDL_Rect r = { cur_x, by, bw - 2, 40 };
        
        SDL_SetRenderDrawColor(ren, (history[i].priority == 0) ? 70 : 180, 120, 180, 255);
        SDL_RenderFillRect(ren, &r);
        draw_text_centered(ren, font, history[i].irq_case, r, (SDL_Color){255,255,255,255});
        cur_x += bw;
    }
}