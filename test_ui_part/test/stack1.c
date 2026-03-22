#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h> // 콘솔 입력을 별도 쓰레드로 받기 위함

#define MAX_STACK_SIZE 100
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// --- 공통 구조체 ---
typedef struct {
    char irq_case[100];
    int priority;
} interrupt_t;

typedef struct {
    interrupt_t stack[MAX_STACK_SIZE];
    int data_count;
} nested_stack_t;

// 전역 변수 (테스트용)
nested_stack_t g_stack = { .data_count = 0 };
int quitting = 0;

// --- [렌더링 팀원의 핵심 함수] ---
void draw_stack(SDL_Renderer *renderer, const nested_stack_t *s) {
    // 스택 가이드 영역
    SDL_Rect area = { 50, 300, 200, 250 };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &area);

    int box_h = 30;
    int margin = 5;
    int start_y = area.y + area.h - box_h - 10;

    // 현재 g_stack에 들어있는 개수만큼 반복 렌더링
    for (int i = 0; i < s->data_count; i++) {
        SDL_Rect box = { area.x + 10, start_y - (i * (box_h + margin)), area.w - 20, box_h };
        
        // 우선순위에 따른 색상 변화 (테스트용)
        SDL_SetRenderDrawColor(renderer, 50 + (i * 30), 100, 200, 255); 
        SDL_RenderFillRect(renderer, &box);
        
        // 테두리
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &box);
    }
}

// --- [콘솔 입력 핸들러] 별도 쓰레드 ---
void* console_thread(void* arg) {
    int cmd;
    printf("\n=== Stack Rendering Test ===\n");
    printf("1: Push dummy data\n2: Pop data\n0: Exit\n");
    
    while (!quitting) {
        printf("> ");
        scanf("%d", &cmd);
        if (cmd == 1) {
            if (g_stack.data_count < MAX_STACK_SIZE) {
                sprintf(g_stack.stack[g_stack.data_count].irq_case, "IRQ_%d", g_stack.data_count);
                g_stack.stack[g_stack.data_count].priority = g_stack.data_count + 1;
                g_stack.data_count++;
                printf("Pushed! Current count: %d\n", g_stack.data_count);
            }
        } else if (cmd == 2) {
            if (g_stack.data_count > 0) {
                g_stack.data_count--;
                printf("Popped! Current count: %d\n", g_stack.data_count);
            }
        } else if (cmd == 0) {
            quitting = 1;
        }
    }
    return NULL;
}

// --- [메인 함수] 호출 및 루프만 담당 ---
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("Stack GUI Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // 콘솔 입력을 위한 쓰레드 시작
    pthread_t thread;
    pthread_create(&thread, NULL, console_thread, NULL);

    SDL_Event e;
    while (!quitting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quitting = 1;
        }

        SDL_SetRenderDrawColor(ren, 30, 30, 30, 255);
        SDL_RenderClear(ren);

        // ★ 렌더링 함수 호출 (데이터 전달)
        draw_stack(ren, &g_stack);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    pthread_join(thread, NULL);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}