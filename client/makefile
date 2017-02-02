PROGRAM_NAME = client
ARGS = localhost 8080 url_list.txt
CC = gcc
LDFLAGS+= -lpthread

client: client.o
	$(CC) -o client client.o $(LDFLAGS)

client.o: 
	$(CC) -c client.c $(LDFLAGS)

run: ${PROGRAM_NAME}
	./${PROGRAM_NAME} ${ARGS}
