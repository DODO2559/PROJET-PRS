# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -g

# Cibles à générer
all: broker client

# Compilation du Broker
broker: broker.c message.h
	$(CC) $(CFLAGS) broker.c -o broker

# Compilation du Client
client: client.c message.h
	$(CC) $(CFLAGS) client.c -o client

# Nettoyage des fichiers binaires
clean:
	rm -f broker client