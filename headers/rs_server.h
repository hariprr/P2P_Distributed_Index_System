#define COOKIE_FILE "../cookie/host_cookie"
#include "../headers/linklist.h"
#include <time.h>

//#define RS_SERVER_IP "10.139.59.129"
//#define RS_SERVER_IP "10.139.56.24"
#define RS_SERVER_IP "192.168.1.6"
#define FAILURE 1
#define SUCCESS 0

typedef struct __peer_info{
    char hostname[20];
    int cookie;
    bool active;
    int ttl;
    int server_port;
    int reg_count;
    struct tm *reg_time;
    time_t reg_temp;
}peer_info;


int send_peer_register_response(int *peer_fd, peer_info *new_peer);
int semd_peer_keepalive_response(int *peer_fd, peer_info *new_peer);
int send_peer_leave_response(int *peer_fd, peer_info *new_peer);
int send_peer_pquery_response(int *peer_fd, peer_info *new_peer, void* this_peer_list);
void free_peer_list(void* this_peer_list);
