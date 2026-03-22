#include "queue.h"
#include "common.h"
#include <stdio.h>
#include <math.h>

void render_queue(SDL_Renderer *ren, TTF_Font *font, priority_queue_t *pq, int offset_x, int offset_y) {
    SDL_Rect area = { 420, 420, 810, 360 }; 
    SDL_SetRenderDrawColor(ren, 35, 35, 40, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 100, 100, 110, 255);
    SDL_RenderDrawRect(ren, &area);

    draw_text_centered(ren, font, "PRIORITY QUEUE (MAX-HEAP TREE)", (SDL_Rect){420, 430, 810, 30}, (SDL_Color){100, 255, 255, 255});

    if (pq->size <= 0) return;

    SDL_Rect clip_area = { area.x + 5, area.y + 45, area.w - 10, area.h - 55 };
    SDL_RenderSetClipRect(ren, &clip_area);

    int node_x[MAX_QUEUE_SIZE], node_y[MAX_QUEUE_SIZE];
    
    // [설계 변경] 각 층(Level)에서 노드가 가질 수 있는 최대 너비를 설정
    // 노드 너비가 110이므로, 리프 노드들 사이의 최소 간격을 130으로 잡습니다.
    int min_node_gap = 130; 
    int root_x = (area.x + area.w / 2) - offset_x;
    int root_y = (area.y + 80) - offset_y; 

    // 1. 모든 노드의 절대 좌표 계산
    for (int i = 0; i < pq->size; i++) {
        int level = 0, temp = i + 1;
        while (temp >>= 1) level++;

        // 해당 레벨에서 몇 번째 노드인지 계산 (0, 1, 2, 3...)
        int pos_in_level = i - ((1 << level) - 1);
        
        // 해당 레벨의 전체 노드 개수 (2^level)
        int nodes_in_level = (1 << level);

        // [핵심 로직] 
        // 전체 트리의 너비를 결정하는 가장 깊은 레벨(예: Level 3) 기준으로 간격을 산출합니다.
        // 레벨이 올라갈수록(루트에 가까울수록) 간격이 2배씩 넓어집니다.
        // MAX_LEVEL을 4 정도로 가정했을 때의 간격 계산입니다.
        int max_level = 3; 
        int spacing = min_node_gap * (1 << (max_level - level));

        // 해당 레벨의 중앙 위치를 root_x로 잡고 좌우로 배치
        float start_x = root_x - ((nodes_in_level - 1) * spacing) / 2.0f;
        
        node_x[i] = (int)(start_x + (pos_in_level * spacing));
        node_y[i] = root_y + (level * 100);

        // 부모와 선 긋기
        if (i > 0) {
            int parent = (i - 1) / 2;
            SDL_SetRenderDrawColor(ren, 80, 80, 90, 255);
            SDL_RenderDrawLine(ren, node_x[parent], node_y[parent], node_x[i], node_y[i]);
        }
    }

    // 2. 노드 렌더링 (두 줄 텍스트)
    for (int i = 0; i < pq->size; i++) {
        SDL_Rect node_rect = { node_x[i] - 55, node_y[i] - 25, 110, 50 };

        set_priority_color(ren, pq->data[i].priority);
        SDL_RenderFillRect(ren, &node_rect);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &node_rect);

        char line1[128], line2[64];
        sprintf(line1, "P%d:%s", pq->data[i].priority, pq->data[i].irq_case);
        sprintf(line2, "(%dms)", pq->data[i].running_time);

        SDL_Rect t_rect = { node_rect.x, node_rect.y + 5, node_rect.w, 20 };
        SDL_Rect b_rect = { node_rect.x, node_rect.y + 25, node_rect.w, 20 };

        draw_text_centered(ren, font, line1, t_rect, (SDL_Color){255, 255, 255, 255});
        draw_text_centered(ren, font, line2, b_rect, (SDL_Color){255, 255, 255, 255});
    }

    SDL_RenderSetClipRect(ren, NULL);
}