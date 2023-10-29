// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network_server.h"
#include "table_server-private.h"
#include "table_skel.h"
int listening_socket;
struct table_t* table;
int main(int argc, char** argv) {
	int port /* , listening_socket */, n_lists;

	// verifica os argumentos passados
	if (argc != 3) {
		printf("Usage: ./table-server <port> <n_lists>\n");
		printf("Example: ./table-server 12345 3\n");
		return -1;
	}

	port = atoi(argv[1]);
	if ((listening_socket = network_server_init(port)) < 0) {
		return -1;
	}

	n_lists = atoi(argv[2]);
	if (n_lists <= 0) {
		return -1;
	}

	/* struct table_t* */ table = table_skel_init(n_lists);

	// Listen to interrupt signal
	signal(SIGINT, table_server_close);

	int result = network_main_loop(listening_socket, table);

	if (result == -1) {
		perror("Erro ao desconectar do servidor\n");
		table_skel_destroy(table);
	}

	return result;
}

void table_server_close(int signum /* , int listening_socket, struct table_t* table */) {
	// Close server
	if (network_server_close(listening_socket) != 0) {
		printf("Error in table_server_close()\n");
	}
	table_skel_destroy(table);
	printf("\nServer Closed Sucessfuly\n");
}
