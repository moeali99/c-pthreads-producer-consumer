.PHONY: all clean
CC=cc
CFLAGS=-O2 -Wall -Wextra -pthread
BIN=bin/app
SRC=src/main.c

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -rf bin