#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/types.h>

#include "client_zookeeper-private.h"

char* root_path = NULL;
char zk_node_id[ZDATALEN];
zhandle_t* zk_connect(const char* address_port, char* rp) {
	// Save root path
	root_path = rp;

	/* Connect to ZooKeeper server */
	int recv_timeout = 2000;
	const clientid_t* client_id = NULL;
	void* context = NULL;
	int flags = 0;
	return zookeeper_init(address_port, NULL, recv_timeout, client_id, context, flags);
}

// Create root node if it doesn't exist
void zk_create_root_node_if_doesnt_exist(zhandle_t* zh) {
	int error = zoo_exists(zh, root_path, 0, NULL);
	if (error == ZNONODE) {
		if (zoo_create(zh, root_path, NULL, -1, &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0) == ZOK) {
			fprintf(stderr, "%s created!\n", root_path);
		} else {
			fprintf(stderr, "Error Creating %s!\n", root_path);
			exit(EXIT_FAILURE);
		}
	}
}

// Register server in ZooKeeper
void zk_register_server(zhandle_t* zh, const char* server_port) {
	char node_path[120] = "";
	strcat(node_path, root_path);
	strcat(node_path, "/node");
	char server_address_port[120];
	char server_address[100];
	get_local_ip(server_address, INET_ADDRSTRLEN);
	sprintf(server_address_port, "%s:%s", server_address, server_port);
	printf("server_address:port: %s\n", server_address_port);
	char* node_metadata = server_address_port;
	int node_metadata_length = strlen(node_metadata) + 1;
	int create_result = zoo_create(zh, node_path, node_metadata, node_metadata_length, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE, zk_node_id, ZDATALEN);
	if (create_result != ZOK) {
		fprintf(stderr, "Error creating znode from path %s!\n", node_path);
		fprintf(stderr, "Error: %d!\n", create_result);
		exit(EXIT_FAILURE);
	}
	strncpy(zk_node_id, zk_node_id + 7, strlen(zk_node_id + 7) + 1);
	printf("Ephemeral Sequencial ZNode created! ZNode path: %s\n", zk_node_id);
}

void zk_get_children(zhandle_t* zh, void* watcher_ctx) {
	// Get the updated children and reset the watch
	zoo_string* children_list = (zoo_string*)malloc(sizeof(zoo_string));
	if (ZOK != zoo_wget_children(zh, root_path, zk_child_watcher, watcher_ctx, children_list)) {
		fprintf(stderr, "Error setting watch at %s!\n", root_path);
	}

	// Call callback function
	((struct watcher_ctx*)watcher_ctx)->callback(children_list, root_path, zh);

	// Free children list
	free(children_list);
}

void zk_child_watcher(zhandle_t* zh, int type, int state, const char* zpath, void* watcher_ctx) {
	if (state == ZOO_CONNECTED_STATE && type == ZOO_CHILD_EVENT) {
		// Get the updated children and reset the watch
		zoo_string* children_list = (zoo_string*)malloc(sizeof(zoo_string));
		if (ZOK != zoo_wget_children(zh, root_path, zk_child_watcher, watcher_ctx, children_list)) {
			fprintf(stderr, "Error setting watch at %s!\n", root_path);
		}

		// Call callback function
		((struct watcher_ctx*)watcher_ctx)->callback(children_list, root_path, zh /* children_list, root_path */);

		// Free children list
		free(children_list);
	}
}

int get_local_ip(char* ip_str, size_t buflen) {
	struct ifaddrs *ifaddr, *ifa;
	int family;
	int found = 0;

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return -1;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {  // Check for IPv4 addresses only
			void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
			inet_ntop(family, addr, ip_str, buflen);

			if (strcmp(ip_str, "127.0.0.1") != 0) {  // Exclude loopback address
				found = 1;
				break;
			}
		}
	}

	freeifaddrs(ifaddr);

	return found ? 0 : -1;
}

void zk_disconnect(zhandle_t* zh) {
	zookeeper_close(zh);
}
