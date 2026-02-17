CC = gcc
CFLAGS = -Wall

all: broker client menu.x

broker client: %: %.c message.h
	$(CC) $(CFLAGS) $< -o $@

menu.x: menu.c
	$(CC) $(CFLAGS) $< -o $@ -lncursesw

clean:
	rm -f broker client menu.x