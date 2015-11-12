CC = gcc
CFLAGS = -g -Wall -W -std=gnu99

all: server

server: server.o input.o my_socket.o respond.o request.o
	$(CC) server.o input.o my_socket.o respond.o request.o -o server

server.o: server.c
	$(CC) $(CFLAGS) -c server.c -o server.o

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c -o input.o

my_socket.o: my_socket.c my_socket.h
	$(CC) $(CFLAGS) -c my_socket.c -o my_socket.o

request.o: request.c request.h
	$(CC) $(CFLAGS) -c request.c -o request.o

respond.o: request.c respond.h
	$(CC) $(CFLAGS) -c respond.c -o respond.o

clean:
	rm -rf *.o server  *~ 
