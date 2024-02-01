all: main

CC = clang
override CFLAGS += -g -pthread

SRC=$(wildcard *.c)

main: $(SRC)
	$(CC) $(CFLAGS) $^ -o "$@"

run: main
	./main

clean:
	rm -f main