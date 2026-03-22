#include "queue.h"
#include <stdio.h>

void render_queue(SDL_Renderer *ren, TTF_Font *font, priority_queue_t *pq) {
    SDL_Rect area = { 420, 420, 810, 360 }; 
    SDL_SetRenderDrawColor(ren, 35, 35, 40, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 100, 100, 110, 255);
    SDL_RenderDrawRect(ren, &area);

    SDL_Color cyan = {100, 255, 255, 255};
    draw_text_centered(ren, font, "PRIORITY QUEUE (MAX-HEAP TREE)", (SDL_Rect){420, 430, 810, 30}, cyan);

    if (pq->size <= 0) return;

    int node_x[MAX_QUEUE_SIZE];
    int node_y[MAX_QUEUE_SIZE];
    int root_x = 420 + (810 / 2);
    int root_y = 480;

    // 1. 좌표 계산 및 선 그리기
    for (int i = 0; i < pq->size; i++) {
        int level = 0;
        int temp = i + 1;
        while (temp >>= 1) level++;

        int offset = 200 / (1 << level); 
        
        if (i == 0) {
            node_x[i] = root_x;
            node_y[i] = root_y;
        } else {
            int parent = (i - 1) / 2;
            if (i % 2 == 1) node_x[i] = node_x[parent] - offset;
            else           node_x[i] = node_x[parent] + offset;
            node_y[i] = root_y + (level * 70);
            
            // 부모와 연결선
            SDL_SetRenderDrawColor(ren, 80, 80, 90, 255);
            SDL_RenderDrawLine(ren, node_x[parent], node_y[parent], node_x[i], node_y[i]);
        }
    }

    // 2. 노드 그리기
    for (int i = 0; i < pq->size; i++) {
        SDL_Rect node_rect = { node_x[i] - 45, node_y[i] - 22, 90, 45 };
        
        set_priority_color(ren, pq->data[i].priority); // 공통 함수 호출
        SDL_RenderFillRect(ren, &node_rect);
        
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &node_rect);

        char buf[128];
        // "P우선순위:이름 (시간)" 형식으로 출력
        sprintf(buf, "P%d:%s (%d)", pq->data[i].priority, pq->data[i].irq_case, pq->data[i].running_time);
        draw_text_centered(ren, font, buf, node_rect, (SDL_Color){255, 255, 255, 255});
        
    }
}