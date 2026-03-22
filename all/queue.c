#include "queue.h"
#include "common.h"
#include <stdio.h>

void render_queue(SDL_Renderer *ren, TTF_Font *font, priority_queue_t *pq, int offset_x, int offset_y) {
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
    
    // root_x와 root_y 모두에 offset을 적용하여 전방향 스크롤 가능
    int root_x = 420 + (810 / 2) - offset_x;
    int root_y = 480 - offset_y;

    // 1. 좌표 계산 및 연결선 그리기
    for (int i = 0; i < pq->size; i++) {
        int level = 0;
        int temp = i + 1;
        while (temp >>= 1) level++;

        // 겹침 방지: 기본 너비를 넓게 잡고 최소 간격 확보
        int base_horz_gap = 420; 
        int horz_gap = base_horz_gap / (1 << level);
        if (horz_gap < 55) horz_gap = 55; 
        
        if (i == 0) {
            node_x[i] = root_x;
            node_y[i] = root_y;
        } else {
            int parent = (i - 1) / 2;
            if (i % 2 == 1) node_x[i] = node_x[parent] - horz_gap;
            else           node_x[i] = node_x[parent] + horz_gap;
            
            node_y[i] = root_y + (level * 90);
            
            // 영역 안에서만 선 그리기
            if (node_x[parent] > area.x && node_x[parent] < area.x + area.w &&
                node_y[parent] > area.y + 60 && node_y[parent] < area.y + area.h &&
                node_x[i] > area.x && node_x[i] < area.x + area.w &&
                node_y[i] > area.y + 60 && node_y[i] < area.y + area.h) {
                SDL_SetRenderDrawColor(ren, 80, 80, 90, 255);
                SDL_RenderDrawLine(ren, node_x[parent], node_y[parent], node_x[i], node_y[i]);
            }
        }
    }

    // 2. 노드 박스 그리기
    for (int i = 0; i < pq->size; i++) {
        // 상하좌우 클리핑 적용
        if (node_x[i] > area.x + 40 && node_x[i] < area.x + area.w - 40 &&
            node_y[i] > area.y + 60 && node_y[i] < area.y + area.h - 30) {
            
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