// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t {
	int size;
	int rows;
	struct list_t** lists;
};

/* Função que calcula o índice da lista a partir da chave
 */
int hash_code(char* key, int module);

#endif
