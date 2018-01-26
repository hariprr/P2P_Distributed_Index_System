
#define REGISTER_REQUEST 0
#define REGISTER_RESPONSE 1
#define LEAVE_REQUEST 2
#define LEAVE_RESPOSNE 3
#define PQUERY_REQUEST 4
#define PQUERY_RESPONSE 5
#define KEEPALIVE_REQUEST 6
#define KEEPALIVE_RESPONSE 7
#define RFC_GET_QUERY 8
#define RFC_GET_QUERY_RESPONSE 9
#define GET_RFC_CONTENT 10
#define TAKE_RFC_CONTENT 11

#define KEEPALIVE_TIMEOUT_INTERVAL 5

#define RS_SERVER_PORT "5000"

// Get list of all files
#define RFC_GET_QUERY_HEADER 1
// Tke list of all files
#define RFC_GET_QUERY_RESPONSE_HEADER 1

// Give me one file
#define RFC_GET_CONTENT_HEADER 7
//Take on file
#define RFC_DATA_HEADER 15


#define RFC_PQUERY_RESPONSE_HEADER 5

const char *register_req = "REGISTER_REQUEST";
const char *register_response = "REGISTER_RESPONSE";
const char *PQuery = "PQUERY_REQUEST";
const char *keepalive_req = "KEEPALIVE_REQUEST";
const char *keepalive_response = "KEEPALIVE_RESPONSE";
const char *leave_req = "LEAVE_REQUEST";
const char *leave_response = "LEAVE_RESPONSE";
#define CHUNK_SIZE 256
