all: main

CC = clang
override CFLAGS += -g -pthread

main: main.c chunk.c mem.c debug.c value.c
	$(CC) $(CFLAGS) main.c chunk.c mem.c debug.c value.c -o "$@"

run: main
	./main

clean:
	rm -f main