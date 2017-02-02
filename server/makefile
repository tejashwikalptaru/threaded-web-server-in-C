PROGRAM_NAME = server
CC = gcc
LDFLAGS+= -lpthread

server: server.o listner.o thpool.o
	$(CC) -o server server.o listner.o thpool.o $(LDFLAGS)

server.o: 
	$(CC) -c server.c $(LDFLAGS)

listner.o: 
	$(CC) -c listner.c $(LDFLAGS)
	
thpool.o: 
	$(CC) -c thpool.c $(LDFLAGS)

run: ${PROGRAM_NAME}
	./${PROGRAM_NAME}
