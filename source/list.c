#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include "entry-private.h"
#include "entry.h"
#include "list-private.h"

struct list_t* list_create() {
	struct list_t* list = malloc(sizeof(struct list_t));

	if (list == NULL) {
		return NULL;
	}

	list->size = 0;
	list->head = NULL;

	return list;
}

int list_destroy(struct list_t* list) {
	if (list == NULL) {
		return -1;
	}

	struct node_t* current = list->head;
	while (current != NULL) {
		struct node_t* next = current->next;
		if (entry_destroy(current->entry) == -1) {
			return -1;
		}
		free(current);
		current = next;
	}

	free(list);
	return 0;
}

int list_add(struct list_t* list, struct entry_t* entry) {
	if (valid_list(list) == -1) {
		return -1;
	}

	if (entry_valid(entry) == -1) {
		return -1;
	}

	if (list->size == 0) {
		list->head = (struct node_t*)malloc(sizeof(struct node_t));
		if (list->head == NULL) {
			return -1;
		}
		list->head->entry = entry;
		list->head->next = NULL;
		list->size = 1;
		return 0;
	}

	if (list->size == 1) {
		int compare = entry_compare(list->head->entry, entry);
		if (compare == -1) {
			struct node_t* new_node = (struct node_t*)malloc(sizeof(struct node_t));
			if (new_node == NULL) {
				return -1;
			}
			new_node->entry = entry;
			new_node->next = list->head;
			list->head = new_node;
			list->size = 2;
			return 0;
		}
		if (compare == 0) {
			if (entry_replace(list->head->entry, entry->key, entry->value) == -1) {
				return -1;
			}
			free(entry);
			list->size = 2;
			return 1;
		}
		if (compare == 1) {
			struct node_t* new_node = (struct node_t*)malloc(sizeof(struct node_t));
			if (new_node == NULL) {
				return -1;
			}
			new_node->entry = entry;
			new_node->next = NULL;
			list->head->next = new_node;
			list->size = 2;
			return 0;
		}
	}

	struct node_t* current = list->head;
	struct node_t* previous = NULL;
	while (current != NULL) {
		struct node_t* next = current->next;

		int compare = entry_compare(current->entry, entry);
		if (compare == -1) {
			struct node_t* new_node = (struct node_t*)malloc(sizeof(struct node_t));
			if (new_node == NULL) {
				return -1;
			}
			new_node->entry = entry;
			new_node->next = current;
			previous->next = new_node;
			list->size++;
			return 0;
		}
		if (compare == 0) {
			if (entry_replace(current->entry, entry->key, entry->value) == -1) {
				return -1;
			}
			free(entry);
			list->size++;
			return 1;
		}
		if (compare == 1) {
			struct node_t* new_node = (struct node_t*)malloc(sizeof(struct node_t));
			if (new_node == NULL) {
				return -1;
			}
			new_node->entry = entry;
			new_node->next = current->next;
			current->next = new_node;
			list->size++;
			return 0;
		}

		previous = current;
		current = next;
	}

	return -1;
}

/* Função que elimina da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int list_remove(struct list_t* list, char* key) {
	if(valid_list(list) == -1) {
		return -1;
	}
	// quando e a cabeca
	// guardar referencia do next e libertar memoria do no atual e colocar como cabeca o next/temp

	struct node_t* father = NULL;
	struct node_t* node = list->head;

	// quando e do segundo no ate ao fim
	do {
		if (strcmp(node->entry->key, key) == 0) {
			if (node->next != NULL) {
				struct node_t* temp = node->next;
				entry_destroy(node->entry);
				free(node->next);
				free(node);
				father->next = temp;
			} else {
				entry_destroy(node->entry);
				free(node->next);
				free(node);
			}
		}
		father = node;
		node = node->next;

	} while (node->next != NULL);

	return 1;
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL se não encontrar a
 * entry ou em caso de erro.
 */
struct entry_t* list_get(struct list_t* list, char* key) {
	struct node_t* node = list->head;
	struct entry_t* entry = NULL;
	while (node->next != NULL) {
		if (strcmp(node->entry->key) == 0) {
			// entry = node->entry;
			// return entry;
			return node->entry;
		}
		node = node->next;
	}
	return NULL;
}

/* Função que conta o número de entries na lista passada como argumento.
 * Retorna o tamanho da lista ou -1 em caso de erro.
 */
int list_size(struct list_t* list) {
	return list->size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
 * lista, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char** list_get_keys(struct list_t* list) {
	// TODO E SUPOSTO O PONTEIRO ANDAR PARA A FRENTE? SE SIM COMO?
	char** keys = malloc((list->size + 1) * sizeof(char*));
	struct node_t* node = list->head;
	int index = 0;
	while (node->next != NULL) {
		keys[index] = malloc(strlen(node->entry->key) + 1);
		memcpy(keys[index], node->entry->key);
		keys[index][strlen(node->entry->key)] = '\0';
		index++;
		node = node->next;
	}
	keys[list->size] = NULL;
}

/* Função que liberta a memória ocupada pelo array de keys obtido pela
 * função list_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int list_free_keys(char** keys) {
	// TODO como ver os erros aqui????
	int rows = (sizeof(keys) / sizeof(*keys));
	for (int i = 0; i < rows; i++) {
		free(keys[i]);
	}
	free(keys);

	return 0;
}

int valid_list(struct list_t* list) {
	if (list == NULL) {
		return -1;
	}

	if (list->size < 0) {
		return -1;
	}

	struct node_t* current = list->head;
	while (current != NULL) {
		struct node_t* next = current->next;
		if (entry_valid(current->entry) == -1) {
			return -1;
		}
		current = next;
	}
	return 0;
}
