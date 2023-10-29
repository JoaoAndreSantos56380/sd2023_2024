// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void initArgsError();

/*
 * Função que fecha o servidor e destroi a table que está associada.
 */
void table_server_close(int signum/* , int listening_socket, struct table_t* table */);
#endif
