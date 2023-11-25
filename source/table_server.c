// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bubble_sort-private.h"
#include "client_stub-private.h"
#include "client_zookeeper-private.h"
#include "network_server.h"
#include "table_server-private.h"
#include "table_skel.h"
int listening_socket;
struct table_t* table;
zhandle_t* zh;

int main(int argc, char** argv) {
	int port /* , listening_socket */, n_lists;

	// verifica os argumentos passados
	if (argc != 3) {
		initArgsError();
		return -1;
	}

	port = atoi(argv[1]);
	if (port <= 0) {
		printf("Invalid port: %s\n", argv[1]);
		initArgsError();
		return -1;
	}

	n_lists = atoi(argv[2]);
	if (n_lists <= 0) {
		printf("Invalid n_lists: %s\n", argv[2]);
		initArgsError();
		return -1;
	}

	if ((listening_socket = network_server_init(port)) < 0) {
		initArgsError();
		return -1;
	}

	/* struct table_t* */ table = table_skel_init(n_lists);

	// Connect to ZooKeeper to forward client requests
	char* root_node = "/chain";
	zh = zk_connect(argv[2], root_node);
	if (zh == NULL) {
		fprintf(stderr, "Error connecting to ZooKeeper!\n");
		exit(EXIT_FAILURE);
	}

	// Create root node if it doesn't exist
	zk_create_root_node_if_doesnt_exist(zh);

	// Register server in ZooKeeper
	zk_register_server(zh, argv[1] /* port */);

	// Get children list
	struct watcher_ctx watcher_ctx;
	watcher_ctx.callback = select_next_server;
	zk_get_children(zh, &watcher_ctx);

	// Listen ao interrupt signal
	signal(SIGINT, table_server_close);

	int result = network_main_loop(listening_socket, table);

	if (result == -1) {
		perror("Erro ao desconectar do servidor\n");
		table_skel_destroy(table);
	}

	return result;
}

void initArgsError() {
	printf("Usage: ./table-server <port> <n_lists>\n");
	printf("Example: ./table-server 12345 3\n");
}

void table_server_close(int signum /* , int listening_socket, struct table_t* table */) {
	// Disconnect from ZooKeeper
	zk_disconnect(zh);
	// Fechar o server
	if (network_server_close(listening_socket) != 0) {
		printf("Error in network_server_close()\n");
	}
	table_skel_destroy(table);
	printf("\nServer Closed Sucessfuly\n");
}

void select_next_server(zoo_string* children_list, char* root_path, zhandle_t* zh) {
	// Process children list
	printf("Callback function was called on the server!\n");
	printf("antes de ordenar\n");
	for (int i = 0; i < children_list->count; i++) {
		printf("%s\n", children_list->data[i]);
	}
	bubble_sort(children_list->data, children_list->count);
	printf("depois de ordenar\n");
	for (int i = 0; i < children_list->count; i++) {
		printf("%s\n", children_list->data[i]);
	}
	int i;
	for (i = 0; i < children_list->count; i++) {
		if (i == 0 && strcmp(children_list->data[i], zk_node_id) == 0) {
			printf("I'm the head!\n");
		}

		// If we find next node that is not the previous one
		if (strcmp(children_list->data[i], zk_node_id) > 0) {
			// Get next node's IP and port
			int watch = 0;
			int node_metadata_length = ZDATALEN;
			char* node_metadata = malloc(node_metadata_length * sizeof(char));
			struct Stat* stat = NULL;
			char node_path[120] = "";
			strcat(node_path, root_path);
			strcat(node_path, "/");
			strcat(node_path, children_list->data[i]);
			if (zoo_get(zh, node_path, watch, node_metadata, &node_metadata_length, stat) != ZOK) {
				fprintf(stderr, "Error getting new node's metadata at %s!\n", root_path);
			}

			// Connect to the next server
			struct rtable_t* temp_next_server = rtable_connect(node_metadata);
			if (temp_next_server == NULL) {
				fprintf(stderr, "Error connecting to the next server %s:%d!\n", temp_next_server->server_address, temp_next_server->server_port);
			} else {
				// If we're already connected
				if (next_server != NULL) {
					rtable_disconnect(next_server);
				}

				// Switch to the new connection
				next_server = temp_next_server;
				fprintf(stdout, "Connected to the next server %s:%d!\n", next_server->server_address, next_server->server_port);
			}

			// We're done, leave cycle
			free(node_metadata);
			break;
		}
	}

	// If we didn't find a node higher than ours, then there's no next node
	if (i == children_list->count) {
		// If we're already connected
		if (next_server != NULL) {
			rtable_disconnect(next_server);
			next_server = NULL;
		}
		printf("I'm the tail!\n");
	}
}
