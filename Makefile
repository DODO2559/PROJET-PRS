CC = gcc
CFLAGS = -Wall

all: broker client_broadcast client_mp menu.x

broker client_broadcast client_mp: %: %.c message.h
	$(CC) $(CFLAGS) $< -o $@

menu.x: menu.c
	$(CC) $(CFLAGS) $< -o $@ -lncursesw

clean:
	rm -f broker client_broadcast client_mp menu.x
	rm -f fifo_*