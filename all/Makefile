CC = gcc
CFLAGS = -lSDL2 -lSDL2_ttf
SRCS = main.c stack.c queue.c timeline.c
TARGET = monitor

all:
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

clean:
	rm -f $(TARGET)