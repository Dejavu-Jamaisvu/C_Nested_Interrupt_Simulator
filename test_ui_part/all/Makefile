# 컴파일러 설정
CC = gcc
CFLAGS = -Wall -g
LIBS = -lSDL2 -lSDL2_ttf

# 실행 파일 이름
TARGET = monitor

# 컴파일할 소스 파일들 (ui_manager.c 추가됨)
SRCS = main.c ui_manager.c stack.c queue.c timeline.c

# 오브젝트 파일들 (.c를 .o로 변경)
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