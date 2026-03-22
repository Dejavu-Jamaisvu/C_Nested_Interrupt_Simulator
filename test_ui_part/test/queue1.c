#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define MAX_HEAP_SIZE 31 
#define LEVEL_HEIGHT 110

typedef struct {
    int priority;
    int arrival_order;
} Node;

// 시각화용 전역 포커스 변수
int focus_a = -1;
int focus_b = -1;

// --- [시각화 엔진 함수들] ---

void get_node_pos(int idx, int *x, int *y) {
    int depth = (int)log2(idx + 1);
    int nodes_in_lvl = (int)pow(2, depth);
    int pos_in_lvl = idx - (nodes_in_lvl - 1);
    int section = SCREEN_WIDTH / nodes_in_lvl;
    *x = section * pos_in_lvl + (section / 2);
    *y = 100 + (depth * LEVEL_HEIGHT);
}

void render_heap_tree(SDL_Renderer *ren, Node *heap, int size) {
    SDL_SetRenderDrawColor(ren, 20, 25, 30, 255);
    SDL_RenderClear(ren);

    for (int i = 0; i < size; i++) {
        int px, py;
        get_node_pos(i, &px, &py);

        // 선 그리기
        SDL_SetRenderDrawColor(ren, 70, 75, 85, 255);
        if (2 * i + 1 < size) {
            int cx, cy; get_node_pos(2 * i + 1, &cx, &cy);
            SDL_RenderDrawLine(ren, px, py, cx, cy);
        }
        if (2 * i + 2 < size) {
            int cx, cy; get_node_pos(2 * i + 2, &cx, &cy);
            SDL_RenderDrawLine(ren, px, py, cx, cy);
        }

        // 노드 그리기
        if (i == focus_a || i == focus_b) SDL_SetRenderDrawColor(ren, 255, 100, 100, 255);
        else SDL_SetRenderDrawColor(ren, 70, 160, 240, 255);

        SDL_Rect r = {px - 22, py - 22, 44, 44};
        SDL_RenderFillRect(ren, &r);

        // 우선순위 막대
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_Rect bar = {px - 8, py + 12, 16, -(heap[i].priority / 3)};
        SDL_RenderFillRect(ren, &bar);
    }
    SDL_RenderPresent(ren);
}

// --- [힙 로직 함수들] ---

bool is_higher(Node a, Node b) {
    if (a.priority > b.priority) return true;
    if (a.priority == b.priority) return a.arrival_order < b.arrival_order;
    return false;
}

void swap(Node *a, Node *b) {
    Node temp = *a;
    *a = *b;
    *b = temp;
}

// 삽입 동작 함수
void push(SDL_Renderer *ren, Node *heap, int *size, int p, int *counter) {
    if (*size >= MAX_HEAP_SIZE) return;

    int cur = (*size)++;
    heap[cur].priority = p;
    heap[cur].arrival_order = (*counter)++;

    // Up-heap 애니메이션 절차
    while (cur > 0) {
        int parent = (cur - 1) / 2;
        focus_a = cur; focus_b = parent;
        render_heap_tree(ren, heap, *size);
        SDL_Delay(400);

        if (is_higher(heap[cur], heap[parent])) {
            swap(&heap[cur], &heap[parent]);
            cur = parent;
        } else break;
    }
    focus_a = focus_b = -1;
}

// 삭제 동작 함수
void pop(SDL_Renderer *ren, Node *heap, int *size) {
    if (*size <= 0) return;

    // 루트와 마지막 교체 애니메이션
    focus_a = 0; focus_b = *size - 1;
    render_heap_tree(ren, heap, *size);
    SDL_Delay(500);

    heap[0] = heap[--(*size)];
    
    int cur = 0;
    // Down-heap 애니메이션 절차
    while (2 * cur + 1 < *size) {
        int left = 2 * cur + 1;
        int right = 2 * cur + 2;
        int target = left;

        if (right < *size && is_higher(heap[right], heap[left])) target = right;

        focus_a = cur; focus_b = target;
        render_heap_tree(ren, heap, *size);
        SDL_Delay(400);

        if (is_higher(heap[target], heap[cur])) {
            swap(&heap[cur], &heap[target]);
            cur = target;
        } else break;
    }
    focus_a = focus_b = -1;
}
// --- [메인 함수: 터미널 제어 모드] ---

int main(int argc, char *argv[]) {
    Node my_priority_queue[MAX_HEAP_SIZE];
    int current_size = 0;
    int arrival_gen = 0;
    int choice = 0;

    // SDL 초기화 (그래픽 창은 시각화를 위해 필요)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    SDL_Window *win = SDL_CreateWindow("Heap Terminal Control", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    srand(time(NULL));

    printf("\n======================================\n");
    printf("   Heap Visualization Terminal Mode\n");
    printf("======================================\n");
    printf(" 1: Push Random Node\n");
    printf(" 2: Push Priority 50 (FIFO Test)\n");
    printf(" 3: Pop Root Node\n");
    printf(" 0: Exit Program\n");
    printf("======================================\n");

    bool quit = false;
    while (!quit) {
        // 1. 터미널 입력 대기
        printf("\n명령을 입력하세요 > ");
        if (scanf("%d", &choice) != 1) break;

        // 2. 입력에 따른 로직 수행
        switch (choice) {
            case 1:
                printf(">> Random 노드 삽입 중...\n");
                push(ren, my_priority_queue, &current_size, (rand() % 85) + 5, &arrival_gen);
                break;
            case 2:
                printf(">> 우선순위 50 노드 삽입 중...\n");
                push(ren, my_priority_queue, &current_size, 50, &arrival_gen);
                break;
            case 3:
                printf(">> 최우선순위 노드 추출(Pop) 중...\n");
                pop(ren, my_priority_queue, &current_size);
                break;
            case 0:
                printf(">> 프로그램을 종료합니다.\n");
                quit = true;
                break;
            default:
                printf("!! 잘못된 입력입니다. (1, 2, 3, 0 중 입력)\n");
                break;
        }

        // 3. 현재 힙 상태 텍스트로도 출력 (디버깅용)
        printf("Current Array: [ ");
        for (int i = 0; i < current_size; i++) {
            printf("%d ", my_priority_queue[i].priority);
        }
        printf("]\n");

        // SDL 창이 응답 없음이 되지 않도록 이벤트 큐를 비워줌
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }
        
        // 최종 상태 렌더링
        render_heap_tree(ren, my_priority_queue, current_size);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}