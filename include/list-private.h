// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "entry.h"

struct node_t {
	struct entry_t* entry;
	struct node_t* next;
};

struct list_t {
	int size;
	struct node_t* head;
};

/* Função que valida uma list.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int valid_list(struct list_t* list);

#endif
