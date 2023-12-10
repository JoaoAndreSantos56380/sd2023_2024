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
	if (argc != 4) {
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

	// Verificar se o argumento passado está no formato <ip>:<port>
	char* args = strdup((char*)argv[3]);
	char* token = strtok((char*)argv[3], ":");
	if (token == NULL) {
        initArgsError();
        return -1;
    }
	char *zk_ip_str = token;
	token = strtok(NULL, ":");
	if (token == NULL) {
        initArgsError();
        return -1;
    }
	char *zk_port_str = token;

	if ((listening_socket = network_server_init(port)) < 0) {
		initArgsError();
		return -1;
	}

	// Verificar se o ip e o porto são válidos
    struct in_addr zk_addr;
    if (inet_pton(AF_INET, zk_ip_str, &zk_addr) != 1) {
        printf("Invalid IP address: %s\n", zk_ip_str);
		initArgsError();
        return -1;
    }
    int zk_port;
    if (sscanf(zk_port_str, "%d", &zk_port) != 1) {
        printf("Invalid port: %s\n", zk_port_str);
        initArgsError();
        return -1;
    }

	/* struct table_t* */ table = table_skel_init(n_lists);

	// Connect to ZooKeeper to forward client requests
	char* root_node = "/chain";
	zh = zk_connect(args, root_node);
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

	// For previous nodes
	struct watcher_ctx prev_watcher_ctx;
	prev_watcher_ctx.callback = select_prev_server;
	zk_get_children(zh, &prev_watcher_ctx);

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
	printf("Usage: ./table-server <port> <n_lists> <ip:port>[From ZooKeeper!]\n");
	printf("Example: ./table-server 12345 3 127.0.0.1:2181\n");
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
	/* printf("Callback function was called on the server!\n");
	printf("antes de ordenar\n");
	for (int i = 0; i < children_list->count; i++) {
		printf("%s\n", children_list->data[i]);
	} */
	bubble_sort(children_list->data, children_list->count);
	/* printf("depois de ordenar\n");
	for (int i = 0; i < children_list->count; i++) {
		printf("%s\n", children_list->data[i]);
	} */
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


void select_prev_server(zoo_string* children_list, char* root_path, zhandle_t* zh) {
	//printf("Callback select_prev_server function was called on the server!\n");
	//printf("children_count: %d\n", children_list->count);
	//printf("zk_node_id: %s\n", zk_node_id);
	struct rtable_t* head_table = NULL;
	// Order the list
	/* printf("antes de ordenar\n");
	for (int i = 0; i < children_list->count; i++) {
		printf("%s\n", children_list->data[i]);
	} */
	bubble_sort(children_list->data, children_list->count);
	/* printf("depois de ordenar\n");
	for (int i = 0; i < children_list->count; i++) {
		printf("%s\n", children_list->data[i]);
	} */
	// Process children list (assumes it's sorted in ascending order)
	//for (int i = 0; i < children_list->count; i++) {
	for (int i = children_list->count - 1; i >= 0; i--) {
		// Find our own position in the list
		//printf("children_list->data[i]: %s\n", children_list->data[i]);
		if (i == 0 && strcmp(children_list->data[i], zk_node_id) == 0) {
			int watch = 0;
			int node_metadata_length = ZDATALEN;
			char* node_metadata = malloc(node_metadata_length * sizeof(char));
			struct Stat* stat = NULL;
			char node_path[120] = "";
			strcat(node_path, root_path);
			strcat(node_path, "/");
			strcat(node_path, children_list->data[i]);
			//printf("node_path: %s\n", node_path);
			if (zoo_get(zh, node_path, watch, node_metadata, &node_metadata_length, stat) != ZOK) {
				fprintf(stderr, "Error getting head's metadata at %s!\n", root_path);
			} else {
				head_table = rtable_connect(node_metadata);
				if (head_table == NULL) {
					fprintf(stderr, "Error connecting to the predecessor server %s:%d!\n", head_table->server_address, head_table->server_port);
				}

				if (head_table != NULL){
					rtable_disconnect(head_table);
				}
			} //!!!!!!!!!! TODO Fazer free do metadata anterior??

		} else if (strcmp(children_list->data[i], zk_node_id) < 0) {
			// If we're not the first node, there is a predecessor
			// Get the metadata of the predecessor
			int watch = 0;
			int node_metadata_length = ZDATALEN;
			char* node_metadata = malloc(node_metadata_length * sizeof(char));
			struct Stat* stat = NULL;
			char node_path[120] = "";
			strcat(node_path, root_path);
			strcat(node_path, "/");
			strcat(node_path, children_list->data[i]);
			//printf("node_path: %s\n", node_path);
			if (zoo_get(zh, node_path, watch, node_metadata, &node_metadata_length, stat) != ZOK) {
				fprintf(stderr, "Error getting predecessor's metadata at %s!\n", root_path);
			} else {
				// Connect to the predecessor server~
				//printf("node_metadata: %s\n", node_metadata);
				struct rtable_t* prev_server = rtable_connect(node_metadata);
				if (prev_server == NULL) {
					fprintf(stderr, "Error connecting to the predecessor server %s:%d!\n", prev_server->server_address, prev_server->server_port);
				} else {
					// "Merge" the tables
					struct entry_t** prev_table_entries = rtable_get_table(prev_server);
					int i = 0;
					if (prev_table_entries != NULL){
						while(prev_table_entries[i] != NULL) {
							struct entry_t* temp_prev_entry = prev_table_entries[i];
							//head table a null. resolver
							/* if(head_table == NULL){
								head_table = rtable_connect(node_metadata);
								if (head_table == NULL) {
									fprintf(stderr, "Error connecting to the head server %s:%d!\n", head_table->server_address, head_table->server_port);
								}
							} */
							//int result = rtable_put(head_table, temp_prev_entry);
							int result = table_put(table, temp_prev_entry->key, temp_prev_entry->value);//rtable_put(table, temp_prev_entry);
							//printf("head_address: %s\n", head_table->server_address);
							//printf("head_port: %d\n", head_table->server_port);
							/* printf("my_address: %s\n", table->server_address);
							printf("my_port: %d\n", table->server_port); */
							if(result == -1) {
								// O que fazer em caso de erro?
								perror("Error putting entries from the predecessor node into the local table.\n");
							}
							i++;
						}
					}
					printf("Successfully merged tables!\n");
					// Disconnect from the predecessor server and from head server
					if (prev_server != NULL){
						rtable_disconnect(prev_server);
					}

					if(head_table != NULL){
						rtable_disconnect(head_table);
					}
				}
			}
			free(node_metadata);
			break;
		} /* else {
			printf("I'm the head, no predecessor!\n");
			break;
		} */
	}
	//printf("I'm the head, no predecessor!\n");
}
