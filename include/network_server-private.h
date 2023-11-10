#ifndef _NETWORK_SERVER_PRIVATE_H
#define _NETWORK_SERVER_PRIVATE_H

#include <pthread.h>
#include <math.h>
struct ThreadArgs {
	int client_socket;
	struct table_t* table;
};

struct ThreadInfo {
	pthread_t pthread;
	int client_socket;
};

void* client_handler(void* arg);
#endif
