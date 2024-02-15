all: main

CC = clang
override CFLAGS += -g -pthread -lm

SRC=$(wildcard *.c)

main: $(SRC)
	$(CC) $(CFLAGS) $^ -o "$@"

main-debug: $(SRC)
	$(CC) $(CFLAGS) -O0 $^ -o "$@"

run: main
	./main

clean:
	rm -f main