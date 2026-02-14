CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = build/program

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf src/*.o build
