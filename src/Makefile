.PHONY: server
HEADER_FILE_PATH="../headers"
client : 
	rm -f socket_client
	gcc socket_client.c linklist.c common.c ${HEADER_FILE_PATH}/linklist.h -lpthread -o socket_client libevent.a
	./copy.sh

server : 
	rm -f server
	gcc server.c linklist.c ${HEADER_FILE_PATH}/linklist.h -lpthread -o server libevent.a

hari:
	rm -f hari_server
	gcc rs_server_parser.c linklist.c ${HEADER_FILE_PATH}/linklist.h ${HEADER_FILE_PATH}/rs_server.h -lpthread -o hari_server libevent.a

