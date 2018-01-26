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
#include<stdlib.h>
#include<fcntl.h>

// TNRC : Libevent
#include<event.h>
#include<sys/time.h>

// Custom header files
#include "../headers/socket_client.h"
#include "../headers/common_include.h"

// TNRC: Header file to get linklist related features
#include "../headers/linklist.h"

peer_info *my_info = NULL;

//	TNRC: This function clears the Rfc index linklist entry 
void free_rfc_index(void *this_RFC_index){

        if(((RFC_index*)this_RFC_index)->RFC_title)
                free(((RFC_index*)this_RFC_index)->RFC_title);
        if(((RFC_index*)this_RFC_index)->hostname)
                free(((RFC_index*)this_RFC_index)->hostname);

        free((RFC_index*)this_RFC_index);
}


int allocate_peer_struct(peer_info** my_info ){
	if ( *my_info )
		return 1;
	*my_info = (peer_info *) malloc( sizeof(peer_info));
	if ( ! my_info ) {
		printf("Peer memory allocation failed\n");
		return 1;
	}
	(*my_info)->cookie = -1 ;
	(*my_info)->hostname = NULL ;
	(*my_info)->myRFC_index = NULL ;
	(*my_info)->RS_conn_fd = 0;
	return 0;
	
}

int check_and_load_cookie(peer_info **my_info){
	struct stat cookie_file_stat;
	int file_desc = 0,cookie_val = -1;
	FILE *fp =NULL;
	file_desc = open(COOKIE_FILE, O_RDONLY);
	if (fstat(file_desc, &cookie_file_stat)){
		printf("No cookie file present of client 1\n");
		return 1;
	}
	close(file_desc);
	fp = fopen(COOKIE_FILE,"r");
	if ( ! fp ) { 
		printf("No cookie file present of client 2\n");
		return 1;
	}
		
	
	fscanf(fp,"%d",&cookie_val);
	if( allocate_peer_struct(my_info) == SUCCESS){
		printf("Setting cookie_val to %d\n",cookie_val);
		//(*my_info)->cookie = cookie_val;
		(*my_info)->cookie = -1;
		printf("Setting done\n");
	}
	return 0;
}

int send_packet(peer_info *my_info, char *buffer ){
	int bytes_sent=-1;
	printf("Final checkpoint : %x %x, first char  %c and length %d\n", buffer[0],buffer[1],buffer[0],strlen(buffer));
	bytes_sent = send(my_info->RS_conn_fd,buffer,strlen(buffer),0);	
	if(bytes_sent != (strlen(buffer))){
		printf("Peer couldnot send register request\n");
		return 1;
	}
	printf("Sent  %s with bytes %d\n",buffer,bytes_sent);
	return 0;
}

int send_rs_register(peer_info *my_info){
//	int msg_type = REGISTER_REQUEST;
//	int msg_type = KEEPALIVE_REQUEST;
	int msg_type = 1234;
	printf("Send %d REGISTER_REQUEST \n", msg_type);
	int send_len = 11;
	char *buffer = (char*) malloc(send_len);
	memset(buffer,0,send_len);
	if ( my_info && my_info->cookie != - 1  ){
		printf("Send Cookie in request\n");
		snprintf(buffer,send_len,"%d%d",msg_type,my_info->cookie);
		printf("buffer %s with len %d\n",buffer,strlen(buffer));
	}
	else{
		printf("Sending fresh request\n");
		snprintf(buffer,sizeof(buffer),"1234\n");
		printf("changed value  : %s with len :%d\n",buffer,strlen(buffer));
	}
	send_packet(my_info,buffer);
	free(buffer);
	return 0;
}	

int send_rs_keepalive_msg(){
	int msg_type = KEEPALIVE_REQUEST; 
	printf("Send KEEPALIVE_REQUEST from here\n");

// TNRC: This argument passing to libevent doesnot work for now
	int send_len = 11;
	char *buffer = (char*) malloc(11);
	if( ! buffer ){
		printf("Could not allocate buffer to send data \n");
		return FAILURE;
	}
	if ( my_info && my_info->cookie != - 1){
		snprintf(buffer,send_len,"%d%d",msg_type,my_info->cookie);
	}
	else{
		snprintf(buffer,send_len,"%d",msg_type);	
	}
	send_packet(my_info,buffer);
	free(buffer);
	return SUCCESS;
}

int send_rs_pquery_msg(peer_info *my_info){
	int msg_type = PQUERY_REQUEST; 
	printf("Send %d PQUERY_REQUEST \n", msg_type);
	int send_len = 1;
	char *buffer = (char*) malloc(sizeof(int) * (send_len + 2));
	if ( my_info && my_info->cookie != - 1){
		snprintf(buffer,send_len,"%d%d",msg_type,my_info->cookie);
		send_len++;
	}
	else
		snprintf(buffer,send_len,"%d",msg_type);
		

	send_packet(my_info,buffer);
	free(buffer);
	return 0;
}

int send_rs_leave_msg(peer_info *my_info){
	int msg_type = LEAVE_REQUEST; 
	printf("Send %d  LEAVE_REQUEST\n", msg_type);
	int send_len = 1;
	char *buffer = (char*) malloc(sizeof(int) * (send_len + 2));
	if ( my_info && my_info->cookie != - 1){
		snprintf(buffer,send_len,"%d%d",msg_type,my_info->cookie);
		send_len++;
	}
	else
		snprintf(buffer,send_len,"%d",msg_type);
		

	send_packet(my_info,buffer);
	free(buffer);
	return 0;
}

int manage_rs_activity(peer_info *my_info){

	int peer_rs_fd,regis_state,found_server=0;
	struct addrinfo hints,*serverinfo,*p;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;			// Create IPv4 soscket
	hints.ai_socktype = SOCK_STREAM;		// TCP socket

	if ( getaddrinfo(RS_SERVER_IP, RS_SERVER_PORT, &hints, &serverinfo) != SUCCESS )
		printf("Failed  gettaddr info on %s\n", RS_SERVER_IP);
	
	for(p = serverinfo; p != NULL; p = p->ai_next) {	
		if ((peer_rs_fd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            		perror("client: socket");
            		continue;
		}
		if (connect(peer_rs_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(peer_rs_fd);
			perror("client: connect");
			continue;
		}
		found_server = 1;
		break;
        }

	if ( found_server != 1 )
		return FAILURE;
	
	if ( peer_rs_fd )
	{
		my_info->RS_conn_fd = dup(peer_rs_fd);
		printf("Sending Register request to server\n");
		if( send_rs_register(my_info) == FAILURE)
			return 1;
		int message_type;
		int recv_byte = recv(peer_rs_fd,&message_type,sizeof(message_type),0);
                        if ( message_type == REGISTER_RESPONSE)
                                printf("Peer got messg %d from RS\n",message_type);

		return SUCCESS;	
	} 
	else{
		printf("Could not create client socket for RS \n");
		return FAILURE;
	}
}



void schedule_keepalive(peer_info *my_info){

	struct event_base *base = event_base_new();
	struct event *ev1;
        struct timeval five_seconds = {2,0};
	ev1 = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, send_rs_keepalive_msg,NULL);
	event_add(ev1, &five_seconds);
	event_base_dispatch(base);

}

int main(){
//	TNRC : Declare a new list here
	list rfc_index_list;
//	TNRC : Initialize the list with its correct node size and free function.. Note add will be done by Memcpy so no code required there
	list_new(&rfc_index_list,sizeof(RFC_index),free_rfc_index);

	check_and_load_cookie(&my_info);
	while ( manage_rs_activity(my_info) != SUCCESS ){
		printf("Peer cant connect to RS server... Will try again in 1 sec");
		sleep(1);
	}
		
//	schedule_keepalive(my_info);

	return 0;
}
