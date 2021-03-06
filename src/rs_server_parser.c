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
#include <regex.h>
#include<fcntl.h>
#include "../headers/rs_server.h"
#include "../headers/common_include.h"
#include "../headers/linklist.h"


int new_cookie = 1;
struct timespec tms;
// timestamp - date and time
time_t mytime, time_now;
struct tm *ptm;
struct timeval start_time, end_time;


char* craft_PQuery_list(list *list, int cookie)
{
    linklist_node *node = list->head;
    peer_info *my_info;
    char *new_str, *hostname, *server_port;
    server_port = malloc(20);

    if(node == NULL)
        return NULL;

    my_info = (peer_info *)node->data;

    char message_overhead[] = ":  \r\n";
    int max_size = (list->node_count) * ((sizeof(message_overhead)) + sizeof(my_info->hostname) + sizeof("hostname: ") +1 + sizeof("server_port: ") + sizeof(my_info->server_port) +1 + 2 + sizeof(PQuery) + 1);
    printf("Count is %d and max size %d\n",list->node_count,max_size);
    if((new_str = malloc(max_size + 1)) != NULL)
    {
        printf("The size of data feild = %lu\n", sizeof(new_str));
    } else {
        printf("malloc failed!\n");
        return NULL;
    }
    memset(new_str,0,sizeof(new_str));

    printf("APpending PQuery Header\n");
    strcat(new_str," PQUERY_RESPONSE \r\n");

    printf("Listing Active Peers\n");
    while(node != NULL ) {
	printf("Check node\n");
        my_info = (peer_info *)node->data;

        // HRP : REMOVE || 1 condition once list is populated;
        if( (my_info->active == TRUE) && (my_info->cookie != cookie) )
        {
		printf("current str: %s and len %d\n",new_str,(int)strlen(new_str));
            hostname = my_info->hostname;

            strcat(new_str," hostname: ");
            strcat(new_str,hostname);
            strcat(new_str," ");
            snprintf(&new_str[strlen(new_str)],max_size-strlen(new_str)," server_port: %d",my_info->server_port);
            //strcat(new_str,server_port);
            strcat(new_str," \r\n");    // sets end of line

        }
        node = node->next;
    }
    strcat(new_str," \r\n");    // set end of message

    printf("The message body: \n%s\n", new_str);
    return new_str;
}


int split_strings(char **string, char splitStrings[100][100])
{
    fflush(NULL);
    printf("Received String of length %zu\n", strlen(*string));
    int str_len = strlen(*string);
    char *str = malloc(str_len);

    strcpy(str, *string);

    //char splitStrings[100][100]; //can store 10 words of 10 characters
    int i,j,cnt;

//    printf("input string length = %d\n", strlen(string));
    j=0; cnt=0;
    for(i=0;i<=(strlen(str));i++)
    {
        // if space or NULL found, assign NULL into splitStrings[cnt]
        if(str[i]==' ' ||str[i]=='\0')
        {
            splitStrings[cnt][j]='\0';
            cnt++;  //for next word
            j=0;    //for next word, init index to 0
        }
        else
        {
            splitStrings[cnt][j]=str[i];    // append the char to output string
            j++;    // move to next char in input string
        }
    }

    return cnt;
}


bool check_cookie_exists(list *list, int cookie)
{
    linklist_node *node = list->head;
    peer_info *my_info;

    bool result = TRUE;
    while(node != NULL && result) {
      //result = iterator(node->data);

        my_info = (peer_info *)node->data;
        if(cookie == my_info->cookie)
        {
            printf("Cookie %d exists - Hostname: %s\n",
                 cookie, my_info->hostname);
            printf("Update peer info \n");
            // HRP : set peer state as active
            // HRP : update peer register count
            // HRP : reset peer timeout
            // HRP : update peer latest register time
            my_info->active = TRUE;
            my_info->ttl = 7200;
            my_info->reg_count += 1;
            //int timestamp = clock_gettime(CLOCK_REALTIME,&tms);
            //printf("Timestamp = %d \n", timestamp);
            //my_info->reg_timestamp = clock_gettime(CLOCK_REALTIME,&tms);
            my_info->reg_time = time(NULL);
            return TRUE;
        }
node = node->next;
    }
    printf("Cookie %d doesn't exist \n", cookie);
    return FALSE;
}


int send_packet( int connfd, char *message )
{
	int bytes_sent=-1;
	//printf("Final checkpoint : %x %x, first char  %c and length %lu\n", buffer[0],buffer[1],buffer[0],strlen(buffer));
    //return 1;

    // HPR : craft_header(message_type);
    // HPR : use global var for cookie
    // HPR : append data(PQuery) with header and send

	printf("Response type : %s\n", message);
    //snprintf(buffer,sizeof(register_response),"%s\n",register_ptr);

    printf("Connfd = %d\n", connfd);
    bytes_sent = send(connfd,message,strlen(message),0);

    if(bytes_sent != (strlen(message))){
	perror("RS couldnot send register response\n");
	return -1;
     }
    return 0;

}

void serve_peer(int connfd, list *peer_list)
{
    int recv_byte = 0, total_size = 0;
    int timeout = 5; // wait for 5 * 0.5 sec and then exit recv()
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 500000000L; // 0.5 sec
    peer_info new_peer;
	memset(&new_peer,0,sizeof(new_peer));
    //memset(recvBuff, '0',sizeof(CHUNK_SIZE));
    char recvBuff[CHUNK_SIZE] = {0};

    while(1){
	    memset(recvBuff,0,sizeof(recvBuff));
	    if ( (recv_byte = read(connfd, recvBuff, sizeof(recvBuff))) <= 0 )
	    {
		printf("Nothing to recv\n");
		return;
	    }

	    else  {
		printf("serving peer\n");
		printf("list node count = %d\n", peer_list->node_count);
		fflush(NULL);
		printf("Received %d bytes\n", recv_byte);
		printf("---- Got String ----\n%s\n", recvBuff);
		printf("\n---- \t ----\n");

		// call message parser
		char *ptr;
		char splitStrings[100][100];
		ptr = (char *)&recvBuff;
		printf("Calling Receive Buffer Parser\n");

		int cnt = split_strings(&ptr, splitStrings);
		int data_field;
		char *request, request_temp[100], *temp, temp_value[100];      // stores the request message type from peer

		//printf("\nOriginal String is: \n%s\n",ptr);
		printf("\nStrings (words=%d) after split by space:\n", cnt);
		for(int i=0; i < cnt; i++)
		{
		    if(i==1)  {
			printf("Found Request type : %s..next %s\n", splitStrings[0],splitStrings[1]);
			request = memcpy(request_temp, splitStrings[i], sizeof(splitStrings[0]));
			// strcpy - relies on NULL character to stop the copy
			//request = strcpy(temp_value, splitStrings[1]);
			printf("Peer Request type = %s\n", request);
		    }
		    // strcspn - finds a character that we want to reject
		    // HRP : Use this info to figure the end of Header
		    // HRP : Two consecutive <cr> <lf> <cr> <lf> mark the end of hdr
		    if(!(strcspn(splitStrings[i], "\r\n")))  {
			// check if next char is again \r\n
			if(strcmp(splitStrings[i], splitStrings[i+1]) == 0)  {
			    data_field = i+2;   //index after the last match is crlf
			    printf("Found two consecutive <cr><lf> <cr><lf> ; end of header\n"  );
			    printf("Data Field starting index: %d\n", data_field);
			    printf("And starting data is : %s\n\n", splitStrings[data_field]);
			}
			continue;
		    }
		    if(!(strcmp(splitStrings[i], "hostname:")))  {
			printf("Found Hostname : %s\n", splitStrings[i+1]);
			temp = strcpy(temp_value, splitStrings[i+1]);
			strcpy(new_peer.hostname, temp);
			printf("new_peer.hostname = %s\n", new_peer.hostname);
		    }
		    if(!(strcmp(splitStrings[i], "cookie:")))  {
			//printf("Found Cookie : %s\n", splitStrings[i+1]);
			new_peer.cookie = atoi( splitStrings[i+1]);
			printf("new_peer.cookie = %d\n", new_peer.cookie);
		    }
		    if(!(strcmp(splitStrings[i], "server_port:")))  {
			//printf("Found peer server port  : %s\n", splitStrings[i+1]);
			new_peer.server_port = atoi( splitStrings[i+1]);
			printf("new_peer.server_port = %d\n", new_peer.server_port);
		    }
		}

		// set peer ttl and active status
		// updates the main list - only though list_append()
		new_peer.active = TRUE;     // if leave request update to FALSE
		new_peer.ttl = 7200;

		// Check if peer exists in the list - Cookie Check
		// peer registering for first time
		if(new_peer.cookie == '\0' && !(strcmp(request, register_req)))
		{
		    new_peer.reg_count += 1;
		    new_peer.cookie=new_cookie;
		    new_cookie += 1;
		    //new_peer.reg_timestamp = clock_gettime(CLOCK_REALTIME,&tms);
            new_peer.reg_time = time(NULL);

		    printf("Appeding new peer into list with cookie# %d\n", new_cookie);
		    list_append(peer_list, &new_peer);
		    printf("Added peer successfully, no. of peers = %d\n", peer_list->node_count);
		    // send_packet(peer_info, message_type, buffer_data)
		    // HRP : uncomment send function
		    //send_packet(&new_peer, connfd,register_response, NULL);
		}
		// Handling non-register request without cookie
		else if(new_peer.cookie == '\0' && (strcmp(request, register_req)))
		{
		    printf("Peer request without a cookie feild! Do not serve, close connection!\n");
		    close(connfd);
		    exit(0);
		}
		// not a new peer
		else if(new_peer.cookie != '\0')
		{
		    printf("check cookie# %d exists in peer list\n", new_peer.cookie);
		    bool is_peer_new = check_cookie_exists(peer_list, new_peer.cookie);
		    //printf("check_cookie_exists returned %d \n", is_peer_new);

		    if(!is_peer_new)
		    {
			printf("Cookie doesn't exist!! \n");
			list_append(peer_list, &new_peer);
			printf("Added peer successfully, no. of peers = %d\n", peer_list->node_count);
		    }
		    else
		    {
			printf("Cookie exists!! \n");
		    }
		}

		printf("Out of cookie check And *** Serving the Peer Request *** \n");

		// check the request type and server accordingly !! calls send_packet()

		if( !(strcmp(request, register_req)))
		{
		    char data_buffer[100] = {0};
		    printf("Client will get cookie : %d\n",new_peer.cookie);
		    snprintf(data_buffer,sizeof(data_buffer)," REGISTER_RESPONSE \r\n cookie: %d \r\n\r\n",new_peer.cookie);
		    printf("Handle REGISTER_RESPONSE here.. data buffer %s\n",data_buffer);
		    if(!(send_packet(connfd,data_buffer )) < 0)
		        perror("Failed to send the REGISTER_RESPONSE");
		}
		if( !(strcmp(request, PQuery)))
		{
		    char data_buffer[100] = "PQUERY_RESPONSE";
		    printf("Handle PQUERY_RESPONSE here\n");
            	    char* pquery_buffer = craft_PQuery_list(peer_list, new_peer.cookie);
		    if( pquery_buffer ){
			  if((send_packet(connfd,pquery_buffer )) < 0)
				perror("Failed to send the REGISTER_RESPONSE");
		    }
		    return ;
		}
		if( !(strcmp(request, keepalive_req)))
		{
		    char data_buffer[100] = "KEEPALIVE_RESPONSE";
		    printf("Handle KEEPALIVE_RESPONSE here\n");
		  //  if(!(send_packet(&new_peer, connfd,data_buffer )))
		    //    perror("Failed to send the REGISTER_RESPONSE");
		}
		if( !(strcmp(request,leave_req)))
		{
		    char data_buffer[100] = "KEEPALIVE_RESPONSE";
		    printf("Handle LEAVE_RESPONSE here\n");
		  //  if(!(send_packet(&new_peer, connfd,data_buffer )))
		    //    perror("Failed to send the REGISTER_RESPONSE");
		}
	//	printf(" request type = %s\n", request);

//		timeout = 5;    // reset timeout
	    }

	    // break out of loop if timed-out and no data is received -
	    // not working as intended
/*	    if ((timeout > 5) && (recv_byte))  {
		printf("Breaking out of while loop \n" );   // need to add a while to listen ?
		return ;

	    }
*/
    }
}


int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int listenfd = 0;/* connfd[100] = {0}*/;
    int reuse=1;
    char sendBuff[1025];
    time_t ticks;
   // fd_set master;
    int fd_max,retval;
    struct timeval tv;

    tv.tv_usec = 0;
    tv.tv_sec = 5;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");


  //  fcntl(listenfd, F_SETFL, O_NONBLOCK);
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    pid_t pid;

    // peer related stuff
    peer_info new_peer;     // intialise a peer variable
    list peer_list, *peer_list_ptr;         // initialize a new list of peers
    printf("Creating a new list - peer_list\n");
    list_new(&peer_list, sizeof(new_peer), NULL);
    //list_new(peer_list_ptr, sizeof(new_peer), NULL);
    printf("peer_list->count = %d\n", peer_list.node_count);
    peer_list_ptr = &peer_list;
    fd_max = listenfd;


    new_peer.reg_time = gettimeofday(&start_time, NULL);
    printf("%ld\n", new_peer.reg_time->tv_sec);

    sleep(10);

    gettimeofday(&end_time, NULL);

    printf("%ld\n", ((gettimeofday(&end_time, NULL)) - (start_time.tv_sec)));


// HPR : TESTING PEER LIST - Delete
    // new peer - 1
    strcpy(new_peer.hostname,"10.10.10.1");
    new_peer.cookie = new_cookie;
    new_peer.active = 1;
    new_peer.ttl = 7200;
    new_peer.server_port = 12345;
    new_peer.reg_count = 1;
    //new_peer.reg_timestamp = 00000;
    printf("Appeding new element into list\n");
    list_append(&peer_list, &new_peer);
    new_cookie++;


/*


    // new_peer - 2
    strcpy(new_peer.hostname,"remotehost");
    new_peer.cookie = new_cookie;
    new_peer.active = 1;
    new_peer.ttl = 7200;
    new_peer.server_port = 65534;
    new_peer.reg_count = 1;
    new_peer.reg_timestamp = 00100;
    printf("Appeding second element into list\n");
    list_append(&peer_list, &new_peer);
    new_cookie++;
    // new_peer - 3
    strcpy(new_peer.hostname,"www.website.com");
    new_peer.cookie = new_cookie;
    new_peer.active = 1;
    new_peer.ttl = 7200;
    new_peer.server_port = 99999;
    new_peer.reg_count = 1;
    new_peer.reg_timestamp = 10100;
    printf("Appeding third element into list\n");
    list_append(&peer_list, &new_peer);
    new_cookie++;

    // print all the list elements ??
    printf("peer_list->count = %d\n", peer_list.node_count);

    // HRP - TEST : uncomment next 3 lines to test
    printf("printing the list\n");
    //char pquery_list;
    //int cookie = 1;
    //char* dummy_return = craft_PQuery_list(&peer_list, cookie);

// HPR : TESTING PEER LIST - Delete till now

*/
// dont do fork()
// if event on listenfd - call accept
// else call serve peer
    int conn_no=0,itr=0;
    char ipaddress[INET_ADDRSTRLEN]={0};

    struct sockaddr addr;
    struct sockaddr_in *s ;
    socklen_t len=sizeof(addr);
   // FD_ZERO(&master);
    //FD_SET(listenfd, &master);
    int new_fd=0;
    while(1)
    {
//	    FD_ZERO(&master);
//	    FD_SET(listenfd, &master);
//	    for(itr=0;itr<conn_no;itr++){
//		FD_SET(connfd[conn_no],&master);
//	     }
//        printf("connfd = %d\n", connfd);
//	retval = select(fd_max+1,&master,NULL,NULL,&tv);
//	if (retval == -1){
//		printf("Socket must have closed... Connection will just wait in loop now\n");
//		break;
//	}
//	else if (retval){
//		printf("Select returned  : %d\n",retval);
		// Accept new connection .. and add in current fds set;
		// Ask send to work in parallel or as accept to work in parallel too?
///          }
//	    for(itr=0;itr<conn_no;itr++){
//		if(FD_ISSET(/*connfd[conn_no]*/ new_fd,&master)){
//			printf("Going in each fd");
//			serve_peer(/*connfd[conn_no]*/ new_fd, peer_list_ptr, new_peer);
//		}
//	     }
//	    if(FD_ISSET(listenfd,&master)){
       	/*connfd[conn_no] */
	new_fd = accept(listenfd, (struct sockaddr*)&addr,&len);
//		if(/*connfd[conn_no]*/ new_fd != -1){
//			conn_no++;
	s = (struct sockaddr_in *)&addr;
	memset(ipaddress,0,sizeof(ipaddress));
	inet_ntop(AF_INET, &s->sin_addr, ipaddress, sizeof(ipaddress));
	printf("Got a connection from %s and gave it fd %d\n",ipaddress,new_fd/*connfd[conn_no]*/);
	serve_peer(/*connfd[conn_no]*/ new_fd, peer_list_ptr);
	close(new_fd);
//			connfd[conn_no]=dup(new_fd);
//			if ( /*connfd[conn_no]*/ new_fd > fd_max){
				//fd_max = connfd[conn_no];
//				fd_max = new_fd;
//			}
//			FD_SET(/*connfd[conn_no]*/ new_fd,&master);
//			conn_no++;
//		}
//	    }

//	}
    }

        // make socket non blocking
        //fcntl(connfd, F_SETFL, O_NONBLOCK);

/*        pid = fork();

        if(pid > 0)
        {
            // parent
            close(connfd);
            continue;
        }

        if(pid == 0)
        {
            // child
*/

        //}
}
