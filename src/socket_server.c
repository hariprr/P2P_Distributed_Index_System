#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<netdb.h>
// Custom header files
#include "../headers/common_include.h"

#define BACKLOG 10
int main(){
	int sockfd, new_fd;
	struct sockaddr_storage conn_addr;
	struct sockaddr hints, *serverinfo,*p;

	memset(&hints , 0 ,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if( getaddrinfo(NULL,RS_SERVER_PORT, &hints ,&serverinfo) != 0){
		exit 1;
	} 
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
		    perror("server: socket");
		    continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		    perror("setsockopt");
		    exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		    close(sockfd);
		    perror("server: bind");
		    continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if (listen(sockfd, BACKLOG) == -1) {
        	perror("listen");
        	exit(1);
	}
	
    	while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
		char buffer[10];
		memset(buffer,0,sizeof(buffer));
		int recv_byte = recv(new_fd,buffer,sizeof(buffer),0);
		printf("Got %d bytes\n",recv_byte);
		int msg_type = RS_REGISTER_RESPONSE;
            if (send(new_fd, (void *)&msg_type, 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_f	
	return 0;
}
