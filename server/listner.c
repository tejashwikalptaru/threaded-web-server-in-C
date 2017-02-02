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
#include "listner.h"

void cerror(FILE *stream, char *cause, char *errno, char *shortmsg, char *longmsg) {
  fprintf(stream, "HTTP/1.1 %s %s\n", errno, shortmsg);
  fprintf(stream, "Content-type: text/html\n");
  fprintf(stream, "\n");
  fprintf(stream, "<html><title>Web server Error</title>");
  fprintf(stream, "<body bgcolor=""ffffff"">\n");
  fprintf(stream, "%s: %s\n", errno, shortmsg);
  fprintf(stream, "<p>%s: %s\n", longmsg, cause);
  fprintf(stream, "<hr><em>Web server</em>\n");
}

void handleRequest(FILE* stream)
{
  char buf[BUFSIZE];
	char method[BUFSIZE];
	char uri[BUFSIZE];
	char version[BUFSIZE];
	char filename[BUFSIZE];
	char filetype[BUFSIZE]; 
	char *p;             
	struct stat sbuf;
	int fd;
	
  fgets(buf, BUFSIZE, stream);
  printf("%s", buf);
  sscanf(buf, "%s %s %s\n", method, uri, version);
    
  if(strlen(uri)<1){
    strcpy(uri, "/index.html");
	}
    

  if(strcasecmp(method, "GET")){
    cerror(stream, method, "501", "Not Implemented", "does not implemented, only GET requests");
    fclose(stream); 
    return;
  }

  fgets(buf, BUFSIZE, stream);
  printf("%s", buf);
  while(strcmp(buf, "\r\n")) {
    fgets(buf, BUFSIZE, stream);
    printf("%s", buf);
  }

  strcpy(filename, ".");
  strcat(filename, uri);
  if (uri[strlen(uri)-1] == '/'){
    if(strlen(uri)<=1)
      strcat(filename, "index.html");
    else
      strcat(filename, uri);
  }
    

  if(stat(filename, &sbuf) < 0) {
    cerror(stream, filename, "404", "Not found", "couldn't find requested file");
    fclose(stream);
    return;
  }

  if(strstr(filename, ".html"))
    strcpy(filetype, "text/html");

  fprintf(stream, "HTTP/1.1 200 OK\n");
  fprintf(stream, "Server: Simple Web Server\n");
  fprintf(stream, "Content-length: %d\n", (int)sbuf.st_size);
  fprintf(stream, "Content-type: %s\n", filetype);
  fprintf(stream, "\r\n"); 
  fflush(stream);

  fd = open(filename, O_RDONLY);
  p = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  fwrite(p, 1, sbuf.st_size, stream);
  munmap(p, sbuf.st_size);
  close(fd);

  fclose(stream);	
}
