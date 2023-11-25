// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H

#include "client_stub-private.h"

extern struct rtable_t* next_server;

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void initArgsError();

/*
 * Função que fecha o servidor e destroi a table que está associada.
 */
void table_server_close(int signum /* , int listening_socket, struct table_t* table */);
void select_next_server(zoo_string* children_list, char* root_path, zhandle_t* zh);
#endif
