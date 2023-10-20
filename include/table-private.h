// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"
#include "table.h"
#include "table-private.h"
struct table_t {
	int size;
	int rows;
	struct list_t** lists;
};

/*
 * Função que calcula o índice da lista a partir da chave
 */
int hash_code(char* key, int module);

/*
 * Função que devolve todas as entries de uma lista.
 * O num_entries deve ser inicializado anteriormente a 0,
 * este vai servir para sabermos quantas entries foram
 * devolvidas.
 */
struct entry_t **get_all_entries(struct table_t *table, int *num_entries);
#endif
