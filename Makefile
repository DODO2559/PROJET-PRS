CC = gcc
CFLAGS = -Wall -g

all: clean broker client_broadcast client_mp client_groupes menu

broker client_broadcast client_mp client_groupes: %: %.c message.h
	$(CC) $(CFLAGS) $< -o $@

menu: menu.c
	$(CC) $(CFLAGS) $< -o $@ -lncursesw

clean:
	rm -f *.o broker client_mp client_broadcast client_groupes menu
	rm -f fifo_*
	rm -f tubebroker
	rm -f /dev/shm/sem.semaphore
	