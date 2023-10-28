// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <table.h>
#include <unistd.h>

#include "message-private.h"
#include "network_server.h"
#include "table_skel.h"


int network_server_init(short port) {
	// socket info struct
	int listening_socket;
	struct sockaddr_in server_info = {0};
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(port);
	socklen_t server_info_len = sizeof(struct sockaddr_in);

	// Ignore SIGPIPE signal so server doesn't crash if socket closes unexpectedly
	struct sigaction new_actn;
	new_actn.sa_handler = SIG_IGN;
	sigemptyset(&new_actn.sa_mask);
	new_actn.sa_flags = 0;
	sigaction(SIGPIPE, &new_actn, NULL);

	// socket
	if ((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}

	int option_value = 1;
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
	// bind to addr
	if (bind(listening_socket, (struct sockaddr*)&server_info, server_info_len) < 0) {
		perror("bind");
		return -1;
	}

	// listen for connections
	if (listen(listening_socket, 0) < 0) {	 // right declaration
		perror("listen failed");
		return -1;
	}
	printf("Listening on port %hu...\n", port);

	return listening_socket;
}


int network_main_loop(int listening_socket, struct table_t* table) {
	struct sockaddr client_info = {0};
	socklen_t client_info_len = sizeof(client_info);
	int client_socket;
	while ((client_socket = accept(listening_socket, &client_info, &client_info_len)) > 0) {
		printf("Client connected\n");

		struct message_t* msg;
		while ((msg = network_receive(client_socket)) != NULL) {
			invoke(msg, table);
			network_send(client_socket, msg);
		}
		close(client_socket);
		printf("Client disconnected\n");
	}
	return 0;
}


struct message_t* network_receive(int client_socket) {
	char* buff = (char*)malloc(1024);
	int size = read_all(client_socket, &buff, 1024);
	struct message_t* result = size > 0 ? message_t__unpack(NULL, size, (uint8_t*)buff) : NULL;
	free(buff);
	return result;
}


int network_send(int client_socket, struct message_t* msg) {
	char* buffer = (char*)malloc(1024);
	int buffer_size = message_t__pack(msg, (uint8_t*)buffer);
	message_t__free_unpacked(msg, NULL);
	int num_bytes_written = write_all(client_socket, buffer, buffer_size);
	free(buffer);
	return num_bytes_written;
}


int network_server_close(int socket) {
	close(socket);
	return 0;
}
