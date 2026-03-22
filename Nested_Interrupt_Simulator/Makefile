# 컴파일러 설정
CC = gcc

# -I 옵션을 사용하여 헤더 파일(.h)을 찾을 폴더 경로들을 지정합니다.
CFLAGS = -Wall -g -I./include -I./src/logic -I./src/ui

# 라이브러리 링크
LIBS = -lSDL2 -lSDL2_ttf

# 실행 파일 이름
TARGET = nested_interrupt_simulator

# 소스 파일(.c)들의 정확한 상대 경로를 지정합니다.
SRCS = src/main.c \
       src/logic/stack.c \
       src/logic/queue.c \
       src/logic/scheduler.c \
       src/ui/ui_main.c \
       src/ui/ui_stack.c \
       src/ui/ui_queue.c

# 오브젝트 파일(.o) 이름 생성
OBJS = $(SRCS:.c=.o)

# 기본 빌드 규칙
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

# 개별 .c 파일을 .o 파일로 컴파일
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 생성된 파일 삭제 (정리용)
clean:
	rm -f $(TARGET) $(OBJS)

# 다시 빌드
re: clean all