#include "stack.h"
#include "common.h"
#include <stdio.h>

void render_stack(SDL_Renderer *ren, TTF_Font *font, nested_stack_t *ns, int offset_y) {
    // 1. 스택 영역 배경 및 테두리 (이건 잘리면 안 됨)
    SDL_Rect area = { 50, 420, 350, 360 }; 
    SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
    SDL_RenderFillRect(ren, &area);
    SDL_SetRenderDrawColor(ren, 70, 70, 80, 255);
    SDL_RenderDrawRect(ren, &area);

    // 타이틀 (클리핑 전 렌더링)
    SDL_Rect title_box = { area.x, area.y + 10, area.w, 30 };
    draw_text_centered(ren, font, "NESTED STACK (LIFO)", title_box, (SDL_Color){255, 255, 100, 255});

    if (ns->data_count <= 0) return;

    // 2. [핵심] 블록들이 그려질 "안쪽 내용물" 영역만 클리핑 설정
    // 타이틀바 아래(area.y + 45)부터 박스 바닥(area.h - 55)까지만 그림이 허용됨
    // 이 영역 밖으로 나가는 그림은 픽셀 단위로 잘려서 안 보입니다. (큐와 동일한 부드러움)
    SDL_Rect clip_area = { area.x + 5, area.y + 45, area.w - 10, area.h - 55 };
    SDL_RenderSetClipRect(ren, &clip_area);

    // 텍스트가 길어질 수 있으므로 block_w를 310으로 약간 키움
    int block_w = 310, block_h = 45, gap = 10;
    int step = block_h + gap;
    int top_y = area.y + 55; // 첫 번째 블록 기준점 (가장 최근 데이터가 올 자리)

    for (int i = 0; i < ns->data_count; i++) {
        // 인덱스 역순: 최상단 데이터(count-1)를 i=0(맨 위)에 배치
        int data_idx = (ns->data_count - 1) - i;
        int y_pos = top_y + (i * step) + offset_y;

        // 화면 근처에 있을 때만 렌더링 시도 (ClipRect가 실제 잘림 처리)
        if (y_pos > area.y - 50 && y_pos < area.y + area.h + 50) {
            SDL_Rect block = { area.x + (area.w - block_w) / 2, y_pos, block_w, block_h };

            // 우선순위별 색상
            set_priority_color(ren, ns->stack[data_idx].priority);
            SDL_RenderFillRect(ren, &block);
            
            // 테두리 그리기
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderDrawRect(ren, &block);

            // [수정 포인트] 텍스트 형식: [우선순위] 이름 (남은시간) + [Top/Bottom]
            char buf[256];
            
            // ns->stack[data_idx].remaining_time 변수명이 다를 경우 확인 필요
            // 만약 running_time이라면 그에 맞게 수정하시면 됩니다.
            sprintf(buf, "[P%d] %s (%dms)", 
                    ns->stack[data_idx].priority, 
                    ns->stack[data_idx].irq_case, 
                    ns->stack[data_idx].running_time); // 멤버 이름 수정 완료

            // 최상단(Top) 판별 (가장 마지막에 들어온 데이터)
            if (data_idx == ns->data_count - 1) {
                sprintf(buf + strlen(buf), " [Top]");
            }
            // 최하단(Bottom) 판별 (가장 처음에 들어온 데이터)
            else if (data_idx == 0) {
                sprintf(buf + strlen(buf), " [Bottom]");
            }

            draw_text_centered(ren, font, buf, block, (SDL_Color){255, 255, 255, 255});
        }
    }

    // 3. [매우 중요] 클리핑 해제!! 
    // 이걸 안 하면 다음에 그려질 큐(Queue)나 타임라인이 스택 박스 위치 안에서만 그려지려고 해서 사라집니다.
    SDL_RenderSetClipRect(ren, NULL);
}