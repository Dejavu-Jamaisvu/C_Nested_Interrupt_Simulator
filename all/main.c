#include "stack.h"
#include "queue.h"
#include "timeline.h"

// 더미 데이터 주입
interrupt_t g_history[5] = { {"Main", 0, 5}, {"Timer", 4, 3}, {"Main", 0, 4} };
nested_stack_t g_stack = { .data_count = 2, .stack = {{"Main", 0, 5}, {"Timer", 4, 3}} };
priority_queue_t g_queue = { .size = 2, .data = {{"UART", 1, 0}, {"DMA", 2, 0}} };

void draw_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect tgt = { box.x + (box.w - surf->w)/2, box.y + (box.h - surf->h)/2, surf->w, surf->h };
    SDL_RenderCopy(ren, tex, NULL, &tgt);
    SDL_FreeSurface(surf); SDL_DestroyTexture(tex);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO); TTF_Init();
    SDL_Window *win = SDL_CreateWindow("Interrupt System Monitor", 100, 100, 1280, 800, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);

    int running = 1; SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;
        SDL_SetRenderDrawColor(ren, 25, 25, 30, 255); SDL_RenderClear(ren);

        render_timeline(ren, font, g_history, 3);
        render_stack(ren, font, &g_stack);
        render_queue(ren, font, &g_queue);

        SDL_RenderPresent(ren);
    }
    return 0;
}