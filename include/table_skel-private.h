// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

extern struct rtable_t* next_server;

/*
 * Função a ser chamada pelo network_server, para atualizar
 * o número de clientes atualmente atualizados pelo servidor.
 *
 * O valor n passado equivale ao número de clientes a serem retirados
 * ou adicionados. O valor op passado equivale ao tipo de operação
 * que vai ser feita, neste caso, se op for:
 * - 0 -> vão ser adicionados n clientes
 * - 1 -> vão ser subtraídos n clientes
 */
void update_server_stats_clients(int n, int op);
void update_stats(struct timeval start_time, struct timeval end_time);

#endif
