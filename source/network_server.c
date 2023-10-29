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
	// Receive size
	short num;
	// int recv_result = recv(client_socket, &num, sizeof(short), 0);
	int recv_result = read(client_socket, &num, sizeof(short));
	if (recv_result == -1) {
		perror("recv error");
		return NULL;
	}
	num = ntohs(num);

	char* buff = (char*)malloc(num);

	// Assume read_all is correct
	int size = read_all(client_socket, &buff, num);
	struct message_t* result = size > 0 ? message_t__unpack(NULL, size, (uint8_t*)buff) : NULL;

	free(buff);
	return result;
}

int network_send(int client_socket, struct message_t* msg) {
	// Send size
	short num = (short)message_t__get_packed_size(msg);
	short num_htons = htons(num);
	// int send_result = send(client_socket, &num_htons, sizeof(short), 0);
	int send_result = write(client_socket, &num_htons, sizeof(short));
	if (send_result == -1) {
		perror("send error");
		return -1;
	}
	char* buffer = (char*)malloc(num);
	if (buffer == NULL) {
		return -1;
	}
	message_t__pack(msg, (uint8_t*)buffer);
	// Assume write_all is correct
	int num_bytes_written = write_all(client_socket, buffer, num);
	if (num_bytes_written != num) {
		perror("write_all error");
		free(buffer);
		return -1;
	}

	message_t__free_unpacked(msg, NULL);  // Free message after sending

	free(buffer);
	return num_bytes_written;
}

int network_server_close(int socket) {
	close(socket);
	return 0;
}
