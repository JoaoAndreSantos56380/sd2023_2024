// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "network_server.h"
#include "table_skel.h"

int main(int argc, char **argv) {
    int port, listening_socket, n_lists;

    // verifica os argumentos passados
    if (argc != 3) {
        printf("Usage: ./table_server <port> <n_lists>\n");
        printf("Example: ./table_server 12345 3\n");
        return -1;
    }

    port = atoi(argv[1]);
    if((listening_socket = network_server_init(port)) < 0) {
        return -1;
    }

    n_lists = atoi(argv[2]);
    if(n_lists <= 0) {
        return -1;
    }

    struct table_t *table = table_skel_init(n_lists);

    int result = network_main_loop(listening_socket, table);

    if(result == -1) {
        perror("Erro ao desconectar do servidor\n");
    }

    table_skel_destroy(table);
    return result;
}
