#define COOKIE_FILE "../cookie/host_cookie"
//#define RS_SERVER_IP "10.139.59.129"
//#define RS_SERVER_IP "10.139.65.216"
//#define RS_SERVER_IP "192.168.1.6"
#define RS_SERVER_IP "127.0.0.1"
#define FAILURE 1
#define SUCCESS 0
#define KEEPALIVE_TIMER 5

#define CHUNK_SIZE 256
typedef struct RFC_index{
	int RFC_num;
	char RFC_title[200];
	char hostname[20];
	int ttl;
}RFC_index;


typedef struct peer_info{
	char hostname[16];
	int cookie;
	int RS_conn_fd;
	char listen_port[6];
	int rfc_server_fd;
	int rfc_client_fd;
	int childpid;
}peer_info;

int send_rs_regsiter(peer_info *my_info);
int send_rs_keepalive_msg(peer_info *my_info);
int send_rs_leave_msg(peer_info *my_info);
int send_rs_pquery_msg(peer_info *my_info);
void free_rfc_index(void* this_RFC_index);
void schedule_keepalive();
int split_strings(char **string, char splitStrings[100][100]);
