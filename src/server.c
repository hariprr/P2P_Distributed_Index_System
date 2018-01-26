#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025]={0};
    time_t ticks; 
    char respBuff[1024]={0};

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 
    memset(respBuff, '0', sizeof(respBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 
int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEPORT) failed");
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    int n=0;
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
	printf("Got connction\n");
        ticks = time(NULL);
//        snprintf(sendBuff, sizeof(sendBuff), "1234\n");
	while ( (n = read(connfd, sendBuff, sizeof(sendBuff)-1)) > 0)
    	{
    	    sendBuff[n] = 0;
		printf("Got string %s\n",sendBuff);
/*
    	    if(fputs(sendBuff, stdout) == EOF)
        	{
        	    printf("\n Error : Fputs error\n");
        	}
*/
		fflush(NULL);
		if (sendBuff[0] == 48){
			printf("Client sent REGISTER request\n");	
			printf("Sending response to client\n");
			int port = atoi(&sendBuff[4]);
			printf("Client port is %d\n",port);
			snprintf(respBuff,sizeof(respBuff),"%d",port);
		 	write(connfd, respBuff, strlen(respBuff)); 
		}
		if (sendBuff[0] == 50){
			printf("Client going down.. Mark it inactive\n");	
		}
		if (sendBuff[0] == 52){
			printf("Client sent PQuery request. . provide response of all active peers\n");
		}
		if (sendBuff[0] == 54){
			printf("Client sent KEEPALIVE request.. update timer\n");	
		}
    	} 	

//        close(connfd);
        sleep(1);
     }
}
