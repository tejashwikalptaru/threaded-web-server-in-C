#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

char *host;
char *portno;
char *filename;

typedef struct {
    char data[4096];
} links;

void request(void* argument)
{
    links *args = argument;	
	char response[4096];
	struct hostent *server;
	unsigned int id = (unsigned int)pthread_self();
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
    if (sockfd < 0) { printf("ERROR opening socket");  pthread_exit(0); }

    server = gethostbyname(host);
    if (server == NULL) { printf("ERROR, no such host");  pthread_exit(0); }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(portno));
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
        printf("ERROR connecting");
        pthread_exit(0);
	}
	
    // send the request
    total = strlen(args->data);
    sent = 0;
    do {
        bytes = write(sockfd,args->data+sent,total-sent);
        if (bytes < 0)
            printf("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);
   

    // receive the response
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0)
            printf("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if(received == total)
        printf("ERROR storing complete response from socket");

    close(sockfd);

    //save response from server
    char file_save[1000];
    sprintf(file_save, "%u%s", id, ".txt"); //convert thread id to char array
    FILE *f = fopen(file_save, "w");
	fputs(response, f);
	fclose(f);
	
    printf("Response from server is saved in file: %s, ThreadID: %u\n\n", file_save, id);
    pthread_exit(0);
}

int main(int argc,char *argv[])
{
	char message[4096];
	char * line = NULL;
    size_t len = 0;
    size_t read = 0;
	FILE* fp;
	int lines = 0;
	int i = 0, j = 0;
	
	if(argc < 3){
        printf("Simple usage:\nclient url port_no file_with_urls\nExample: client localhost 80 file.txt\n\n");
		exit(-1);
	}
	
	host = argv[1];
	portno = argv[2];
	filename = argv[3];
	
	
	fp = fopen(filename, "r");
    if (fp == NULL){
		printf("unable to open file, %s\n", filename);
        exit(-1);
	}
	
	while ((read = getline(&line, &len, fp)) != -1) {
		lines++;
    }
    rewind(fp);
    
    //allocate space
    links *args = malloc(sizeof(links)*lines);
    pthread_t threads[lines+1];
    int* ptr[lines+1];

        
	while ((read = getline(&line, &len, fp)) != -1) {
		
		printf("URL: %s", line);
		printf("Creating thread for request...\n");
		
		strcpy(message, "GET ");
		strcat(message, line);
		strcat(message, " HTTP/1.1\r\n");
		strcat(message, "Host: ");
		strcat(message, host);
		strcat(message, ":");
		strcat(message, portno);
		strcat(message, "\r\n\r\n");
		
		strcpy(args[i].data, message);
		pthread_create(&threads[i], NULL, (void*)request, &args[i]); //create thread
		i++;
		memset(message, 0, sizeof(message)); //then clears the array for next request round
    }
    
    //now lets wait for threads to complete
    for(j=0;j<i;j++)
		pthread_join(threads[j], (void**)&(ptr[j]));
    
    return 0;
}
