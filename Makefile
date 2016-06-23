CC=gcc
CFLAGS=-Wall -g -I -lpthread
TCPchat:: main.c chat.c server.c
	$(CC) -g -o TCPchat main.c -lpthread chat.c client.c -I. 
clean:
	rm TCPchat
