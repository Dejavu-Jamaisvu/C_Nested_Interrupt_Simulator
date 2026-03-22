#include "queue.h"
#include "common.h"
#include <stdio.h>

void render_queue(SDL_Renderer *ren, TTF_Font *font, priority_queue_t *pq, int offset_x, int offset_y) {
    SDL_Rect area = { 420, 420, 810, 360 }; 
    SDL_SetRenderDrawColor(ren, 35, 35, 40, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 100, 100, 110, 255);
    SDL_RenderDrawRect(ren, &area);

    draw_text_centered(ren, font, "PRIORITY QUEUE (MAX-HEAP TREE)", (SDL_Rect){420, 430, 810, 30}, (SDL_Color){100, 255, 255, 255});

    if (pq->size <= 0) return;

    int node_x[MAX_QUEUE_SIZE], node_y[MAX_QUEUE_SIZE];
    int root_x = (area.x + area.w / 2) - offset_x;
    int root_y = (area.y + 80) - offset_y; 

    for (int i = 0; i < pq->size; i++) {
        int level = 0, temp = i + 1;
        while (temp >>= 1) level++;

        int horz_gap = 420 / (1 << level);
        if (horz_gap < 55) horz_gap = 55; 
        
        if (i == 0) {
            node_x[i] = root_x; node_y[i] = root_y;
        } else {
            int parent = (i - 1) / 2;
            node_x[i] = (i % 2 == 1) ? node_x[parent] - horz_gap : node_x[parent] + horz_gap;
            node_y[i] = root_y + (level * 90);
            
            if (node_x[parent] > area.x && node_x[parent] < area.x + area.w &&
                node_y[parent] > area.y + 60 && node_y[parent] < area.y + area.h &&
                node_x[i] > area.x && node_x[i] < area.x + area.w &&
                node_y[i] > area.y + 60 && node_y[i] < area.y + area.h) {
                SDL_SetRenderDrawColor(ren, 80, 80, 90, 255);
                SDL_RenderDrawLine(ren, node_x[parent], node_y[parent], node_x[i], node_y[i]);
            }
        }
    }

    for (int i = 0; i < pq->size; i++) {
        if (node_x[i] > area.x + 40 && node_x[i] < area.x + area.w - 40 &&
            node_y[i] > area.y + 60 && node_y[i] < area.y + area.h - 35) {
            SDL_Rect node_rect = { node_x[i] - 45, node_y[i] - 22, 90, 44 };
            set_priority_color(ren, pq->data[i].priority);
            SDL_RenderFillRect(ren, &node_rect);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &node_rect);
            char buf[128];
            sprintf(buf, "P%d:%s", pq->data[i].priority, pq->data[i].irq_case);
            draw_text_centered(ren, font, buf, node_rect, (SDL_Color){255, 255, 255, 255});
        }
    }
}