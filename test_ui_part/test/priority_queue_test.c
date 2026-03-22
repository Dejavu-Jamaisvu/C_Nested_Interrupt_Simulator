#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define MAX_HEAP_SIZE 31 // 5 levels deep (1+2+4+8+16)
#define NODE_RADIUS 25
#define LEVEL_HEIGHT 100

// 1. 데이터 구조체: FIFO 보장을 위한 arrival_order 포함
typedef struct {
    int priority;      // 우선순위값
    int arrival_order; // 들어온 순서
} Node;

Node heap[MAX_HEAP_SIZE];
int heap_size = 0;
int global_arrival_counter = 0; // 전역 카운터

// 시각화 전용: 현재 포커스된(비교 중인) 노드 인덱스
int focus_node_a = -1;
int focus_node_b = -1;

// 원을 그리는 유틸리티 함수 (SDL2 기본 함수가 아니어서 구현)
void draw_circle(SDL_Renderer *renderer, int x, int y, int radius, bool fill) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal distance from center
            int dy = radius - h; // vertical distance from center
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                if (fill) SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                else if ((dx * dx + dy * dy) > ((radius-2) * (radius-2))) // shell
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

// 힙의 특정 인덱스 노드의 화면 좌표 계산
void get_node_position(int index, int *x, int *y) {
    int level = 0;
    int temp_index = index;
    while (temp_index > 0) {
        temp_index = (temp_index - 1) / 2;
        level++;
    }

    int nodes_in_level = (int)pow(2, level);
    int position_in_level = index - (nodes_in_level - 1);

    // 가로 폭을 레벨에 따라 등분
    int section_width = SCREEN_WIDTH / nodes_in_level;
    *x = position_in_level * section_width + (section_width / 2);
    *y = 50 + level * LEVEL_HEIGHT;
}

// 화면 랜더링 함수 (매 단계마다 호출)
void render_heap(SDL_Renderer *renderer) {
    // 배경색 (검은색)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (heap_size == 0) {
        SDL_RenderPresent(renderer);
        return;
    }

    // 1. 선 그리기 (부모-자식 연결)
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // 회색 선
    for (int i = 0; i < heap_size; i++) {
        int left_child = 2 * i + 1;
        int right_child = 2 * i + 2;

        int px, py, cx, cy;
        get_node_position(i, &px, &py);

        if (left_child < heap_size) {
            get_node_position(left_child, &cx, &cy);
            SDL_RenderDrawLine(renderer, px, py, cx, cy);
        }
        if (right_child < heap_size) {
            get_node_position(right_child, &cx, &cy);
            SDL_RenderDrawLine(renderer, px, py, cx, cy);
        }
    }

    // 2. 노드 그리기
    for (int i = 0; i < heap_size; i++) {
        int nx, ny;
        get_node_position(i, &nx, &ny);

        // 노드 색상 결정
        if (i == focus_node_a || i == focus_node_b) {
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255); // 비교 중: 빨간색
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255); // 일반: 파란색
        }

        draw_circle(renderer, nx, ny, NODE_RADIUS, true);

        // 우선순위값 텍스트 대용 (간단하게 선으로 크기 표현)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        int rect_w = 20;
        int rect_h = heap[i].priority / 2; // 우선순위가 높을수록 긴 막대
        if (rect_h > 40) rect_h = 40;
        
        // 데이터 표시 (우선순위, 도착순서)
        SDL_Rect data_rect = {nx - rect_w/2, ny - rect_h/2, rect_w, rect_h};
        SDL_RenderFillRect(renderer, &data_rect);
    }

    SDL_RenderPresent(renderer);
}

// 유틸리티: 노드 스왑 및 시각화 업데이트
void swap_nodes(SDL_Renderer *renderer, int i, int j) {
    focus_node_a = i;
    focus_node_b = j;
    render_heap(renderer); // 비교 상태 보여주기
    SDL_Delay(300); // 0.3초 대기

    Node temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;

    render_heap(renderer); // 스왑된 상태 보여주기
    SDL_Delay(200);
    focus_node_a = -1;
    focus_node_b = -1;
}

// 핵심 비교 로직 (FIFO 보장)
// return true if node a has higher priority than node b
bool is_higher_priority(Node a, Node b) {
    if (a.priority > b.priority) return true;
    if (a.priority == b.priority) {
        // 우선순위가 같으면 먼저 들어온 것(arrival_order가 작은 것)이 우선
        return a.arrival_order < b.arrival_order;
    }
    return false;
}

// 힙 삽입 (Enqueue) 시 시각화
void heap_push_viz(SDL_Renderer *renderer, int priority) {
    if (heap_size >= MAX_HEAP_SIZE) return;

    // 1. 마지막 위치에 추가
    int current = heap_size;
    heap[current].priority = priority;
    heap[current].arrival_order = global_arrival_counter++;
    heap_size++;

    render_heap(renderer);
    SDL_Delay(500);

    // 2. Up-heap (Reheapify up)
    while (current > 0) {
        int parent = (current - 1) / 2;
        
        // 자식이 부모보다 우선순위가 높으면 스왑
        if (is_higher_priority(heap[current], heap[parent])) {
            swap_nodes(renderer, current, parent);
            current = parent;
        } else {
            // 비교는 했으나 스왑 안 함
            focus_node_a = current;
            focus_node_b = parent;
            render_heap(renderer);
            SDL_Delay(300);
            focus_node_a = -1;
            focus_node_b = -1;
            break;
        }
    }
}

// 힙 삭제 (Dequeue/Sort) 시 시각화
void heap_pop_viz(SDL_Renderer *renderer) {
    if (heap_size <= 0) return;

    printf("Pop: Priority %d (Order %d)\n", heap[0].priority, heap[0].arrival_order);

    // 1. 루트와 마지막 노드 교체
    swap_nodes(renderer, 0, heap_size - 1);
    heap_size--; // 마지막 노드(구 루트) 제거

    render_heap(renderer);
    SDL_Delay(500);

    // 2. Down-heap (Reheapify down / Heapify)
    int current = 0;
    while (true) {
        int left_child = 2 * current + 1;
        int right_child = 2 * current + 2;
        int highest = current;

        // 왼쪽 자식과 비교
        if (left_child < heap_size) {
            if (is_higher_priority(heap[left_child], heap[highest])) {
                highest = left_child;
            }
        }

        // 오른쪽 자식과 비교
        if (right_child < heap_size) {
            if (is_higher_priority(heap[right_child], heap[highest])) {
                highest = right_child;
            }
        }

        if (highest != current) {
            swap_nodes(renderer, current, highest);
            current = highest;
        } else {
            break; // 정렬 완료
        }
    }
    render_heap(renderer);
}

int main(int argc, char *argv[]) {
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Priority Queue (FIFO Heap) Visualization",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // 데모 실행
    bool quit = false;
    SDL_Event e;

    printf("Press '1' to add random node\n");
    printf("Press '2' to add duplicate priority (test FIFO)\n");
    printf("Press '3' to pop (sort) node\n");
    printf("Press 'QUIT' to exit\n");

    // 기본 데이터 몇 개 넣고 시작
    heap_push_viz(renderer, 50);
    heap_push_viz(renderer, 80);
    heap_push_viz(renderer, 30);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_1:
                        // 랜덤 삽입
                        heap_push_viz(renderer, (rand() % 90) + 10);
                        break;
                    case SDLK_2:
                        // 같은 우선순위 삽입 (FIFO 테스트)
                        printf("Inserting Priority 50 again...\n");
                        heap_push_viz(renderer, 50);
                        break;
                    case SDLK_3:
                        // 팝 (정렬)
                        heap_pop_viz(renderer);
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
        }
        render_heap(renderer); // 루프 유지
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
