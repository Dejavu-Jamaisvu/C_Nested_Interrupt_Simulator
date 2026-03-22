#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> // ★ SDL_ttf 라이브러리 추가
#include <stdio.h>
#include <string.h>

#define MAX_BLOCKS 100
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 480
#define TICK_WIDTH 25
#define VISIBLE_X 100
#define VISIBLE_W 1080
#define RED_LINE_X (VISIBLE_X + VISIBLE_W - 150)

// --- 데이터 구조 (이름 추가) ---
typedef struct {
    char task_name[20]; // ★ 블록 위에 표시할 이름
    int task_type;      // 0: Main, 1: ISR1, 2: ISR2
    int duration;
    int is_completed;
} block_t;

block_t g_history[MAX_BLOCKS];
int g_block_count = 0;
int g_total_ticks = 0;

// ★ 글로벌 폰트 변수
TTF_Font *g_font = NULL;

// --- [도우미 함수: 박스 중앙에 텍스트 그리기] ---
void draw_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text, SDL_Rect box, SDL_Color color) {
    if (!font || !text || text[0] == '\0') return;

    // 1. 텍스트를 Surface로 렌더링 (품질 우선: Blended)
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;

    // 2. Surface를 Texture로 변환
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface); // Surface는 이제 필요 없음

    if (!texture) return;

    int tex_w, tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);

    // 3. 박스 중앙 좌표 계산
    SDL_Rect text_rect = {
        box.x + (box.w - tex_w) / 2, // 가로 중앙
        box.y + (box.h - tex_h) / 2, // 세로 중앙
        tex_w,
        tex_h
    };

    // 4. 렌더링 및 정리
    SDL_RenderCopy(ren, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);
}

// --- [렌더링 함수: 텍스트 포함] ---
void draw_timeline_with_text(SDL_Renderer *ren) {
    // 1. 배경 및 틀 그리기
    SDL_Rect bg = { VISIBLE_X, 100, VISIBLE_W, 300 };
    SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
    SDL_RenderFillRect(ren, &bg);

    // 2. 스크롤 오프셋 계산
    int total_w = g_total_ticks * TICK_WIDTH;
    int offset_x = 0;
    if (total_w > (RED_LINE_X - VISIBLE_X)) {
        offset_x = total_w - (RED_LINE_X - VISIBLE_X);
    }

    // 영역 밖 그리기 Clip
    SDL_RenderSetClipRect(ren, &bg);

    int current_x_offset = 0;
    for (int i = 0; i < g_block_count; i++) {
        // 실제 그릴 좌표 계산
        int x = VISIBLE_X + (current_x_offset * TICK_WIDTH) - offset_x;
        int w = g_history[i].duration * TICK_WIDTH;
        
        // 우선순위(task_type)에 따라 높이(Y) 조절 (계단식)
        int y = 300 - (g_history[i].task_type * 70); 

        SDL_Rect r = { x, y, w - 2, 50 }; // 블록 사이 간격 2px
        
        // 블록 색상 결정
        if (g_history[i].task_type == 0) SDL_SetRenderDrawColor(ren, 70, 130, 180, 255);
        else if (g_history[i].task_type == 1) SDL_SetRenderDrawColor(ren, 200, 150, 50, 255);
        else SDL_SetRenderDrawColor(ren, 200, 80, 80, 255);
        
        SDL_RenderFillRect(ren, &r);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); // 테두리 흰색
        SDL_RenderDrawRect(ren, &r);

        // ★ [텍스트 그리기 호출] ★
        SDL_Color text_color = { 255, 255, 255, 255 }; // 흰색 글자
        draw_text_centered(ren, g_font, g_history[i].task_name, r, text_color);

        current_x_offset += g_history[i].duration;
    }

    // 3. 빨간 선 (현재 시점 가이드)
    int final_red_x = VISIBLE_X + (g_total_ticks * TICK_WIDTH) - offset_x;
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    SDL_RenderDrawLine(ren, final_red_x, bg.y, final_red_x, bg.y + bg.h);

    SDL_RenderSetClipRect(ren, NULL); // 클립 해제
}

// --- [가상 데이터 업데이트 타이머] ---
Uint32 update_tick(Uint32 interval, void *param) {
    if (g_block_count >= MAX_BLOCKS) return interval; // 테스트용 개수 제한

    g_total_ticks++;
    if (g_block_count == 0 || g_history[g_block_count-1].is_completed) {
        // 새 블록 생성 (이름 부여)
        int type = rand() % 3;
        g_history[g_block_count].task_type = type;
        if (type == 0) strcpy(g_history[g_block_count].task_name, "Main");
        else if (type == 1) strcpy(g_history[g_block_count].task_name, "ISR1");
        else strcpy(g_history[g_block_count].task_name, "ISR2");

        g_history[g_block_count].duration = 1;
        g_history[g_block_count].is_completed = 0;
        g_block_count++;
    } else {
        g_history[g_block_count-1].duration++;
        // 랜덤하게 블록 완료 (인터럽트 발생/종료 시뮬레이션)
        if (rand() % 8 == 0) g_history[g_block_count-1].is_completed = 1;
    }
    return interval;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    
    // ★ [SDL_ttf 초기화] ★
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Window *win = SDL_CreateWindow("Timeline with Text Test", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // ★ [폰트 로드] ★
    // 예: 리눅스 기본 나눔고딕 폰트 경로 (시스템에 맞춰 수정하세요)
    // 윈도우라면 "C:\\Windows\\Fonts\\arial.ttf" 등
    g_font = TTF_OpenFont("/usr/share/fonts/truetype/nanum/NanumGothic.ttf", 16);
    if (!g_font) {
        printf("Font Load Error: %s\n", TTF_GetError());
        // 폰트가 없어도 사각형은 그리도록 진행
    }

    SDL_AddTimer(200, update_tick, NULL); // 0.2초마다 1틱씩 진행

    int running = 1; SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;
        SDL_SetRenderDrawColor(ren, 20, 20, 25, 255);
        SDL_RenderClear(ren);
        
        draw_timeline_with_text(ren);
        
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    // ★ [정리 호출] ★
    if (g_font) TTF_CloseFont(g_font);
    TTF_Quit();

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}