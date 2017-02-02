#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "thpool.h"
#include "listner.h"

void error(char *msg) {
  perror(msg);
  exit(1);
}


int main()
{
  int parentfd;
  int childfd;
  int portno;
  socklen_t clientlen;
  int optval;
  struct sockaddr_in serveraddr;
  struct sockaddr_in clientaddr;
  threadpool thpool;
  int queue = 0;
  int line_no = 0;
  char * line = NULL;
  size_t len = 0;
  size_t read = 0;
  FILE* config;
  
  config = fopen("server.conf", "r");
  if(config == NULL){
	printf("unable to open config file 'server.conf'");
    exit(-1);
  }
  
  while((read = getline(&line, &len, config)) != -1){
	if(line_no == 0){
		portno = atoi(line);
	}
	if(line_no == 1){
		queue = atoi(line);
	}
	line_no++;
  }
  fclose(config);
  printf("\nServer started\nPort: %d, Thread Pools: %d\n", portno, val);
  

  FILE *stream;
  thpool = thpool_init(val);

  parentfd = socket(AF_INET, SOCK_STREAM, 0);
  if(parentfd < 0) 
    error("ERROR opening socket");

  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);
  if(bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  if(listen(parentfd, queue) < 0)
    error("ERROR on listen");

  clientlen = sizeof(clientaddr);
  while (1)
  {
    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    if (childfd < 0) 
      error("ERROR on accept");
    
    if ((stream = fdopen(childfd, "r+")) == NULL)
      error("ERROR on fdopen");
      
    thpool_add_work(thpool, (void*)handleRequest, (void*)stream);
  }
}
