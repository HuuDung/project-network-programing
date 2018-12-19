CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

server: server.o protocol.o tool.o valid.o account.o
	${CC} protocol.o tool.o valid.o account.o server.o -o server

server.o: server.c
	${CC} ${CFLAGS} server.c

client: client.o protocol.o tool.o valid.o
	${CC} protocol.o tool.o valid.o client.o -o client

client.o: client.c 
	${CC} ${CFLAGS} client.c

protocol.o: protocol.c
	${CC} ${CFLAGS} protocol.c

tool.o: tool.c
	${CC} ${CFLAGS} tool.c

account.o: account.c
	${CC} ${CFLAGS} account.c

valid.o: valid.c
	${CC} ${CFLAGS} valid.c
clean:
	rm -f *.o *~