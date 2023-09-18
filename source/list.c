//#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <list-private.h>

/* Função que cria e inicializa uma nova lista (estrutura list_t a
 * ser definida pelo grupo no ficheiro list-private.h).
 * Retorna a lista ou NULL em caso de erro.
 */
struct list_t* list_create(){
	struct list_t* list = malloc(sizeof(struct list_t));

	if(list == NULL){
		return NULL;
	}

	list->size = 0;
	list->head = malloc(sizeof(struct node_t));

	if(list->head == NULL){
		return NULL;
	}

	list->head->entry = malloc(sizeof(struct entry_t));

	if (list->head->entry == NULL) {
		return NULL;
	}

	list->head->next = NULL;

	return NULL;
}

/* Função que elimina uma lista, libertando *toda* a memória utilizada
 * pela lista.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int list_destroy(struct list_t* list){

	if (list->size == 0) {
		free(list->head);
		free(list);
	}

	struct node_t* node = list->head;

	for (size_t i = 1; i < list->size; i++) {
		entry_destroy(node->entry);
		node = node->next;
	}

	free(list);
	return 0;


}

/* Função que adiciona à lista a entry passada como argumento.
 * A entry é inserida de forma ordenada, tendo por base a comparação
 * de entries feita pela função entry_compare do módulo entry e
 * considerando que a entry menor deve ficar na cabeça da lista.
 * Se já existir uma entry igual (com a mesma chave), a entry
 * já existente na lista será substituída pela nova entry,
 * sendo libertada a memória ocupada pela entry antiga.
 * Retorna 0 se a entry ainda não existia, 1 se já existia e foi
 * substituída, ou -1 em caso de erro.
 */
int list_add(struct list_t* list, struct entry_t* entry){
	//caso em que a lista esta vazia
	if (list->size == 0){
		list->head = entry;
		list->head->next = NULL;
	}

	// caso em que a lista tem um elemento
	struct node_t* node = list->head;
	if (list->size == 1) {
		int result = entry_compare(node->entry, entry);
		if (result == 0) {
			entry_destroy(node->entry);
			node->entry = entry;
		} else if (result == -1) {
			node->next = entry;
		} else if (result == 1) {
			struct node_t* new_node = malloc(sizeof(struct node_t));
			list->head = new_node;
			new_node->next = node;
		} else{
			return -1;
		}
	}

	//caso em que tem 2 ou mais
	struct node_t* son = node->next;
	do {
		if(entry_compare(son->entry, entry) == 0){
			entry_destroy(son->entry);
			son->entry = entry;
			return 1;
		} else if(entry_compare(son->entry, entry) == -2){
			return -1;
		} else if (entry_compare(son->entry, entry)== 1) {
			struct node_t* new_node = malloc(sizeof(struct node_t));
			new_node->entry = entry;
			node->next = new_node;
			new_node->next = son;
			return 0;
		} else if(entry_compare(son->entry, entry) == -1){
			struct node_t* new_node = malloc(sizeof(struct node_t));
			struct node_t* grandson = son->next;
			new_node->entry = entry;
			son->next = new_node;
			new_node->next = grandson;
			return 0;
		}

		node = son;
		son = son->next;
	} while ( son->next != NULL );

	return -1;

}

/* Função que elimina da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int list_remove(struct list_t* list, char* key){

	//quando e a cabeca
	//guardar referencia do next e libertar memoria do no atual e colocar como cabeca o next/temp

	struct node_t* father = NULL;
	struct node_t* node = list->head;

	//quando e do segundo no ate ao fim
	do {
		if (strcmp(node->entry->key, key) == 0) {
			if (node->next != NULL) {
				struct node_t* temp = node->next;
				entry_destroy(node->entry);
				free(node->next);
				free(node);
				father->next = temp;
			} else{
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
struct entry_t* list_get(struct list_t* list, char* key){
	struct node_t* node = list->head;
	struct entry_t* entry = NULL;
	while (node->next != NULL) {
		if (strcmp(node->entry->key)== 0) {
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
int list_size(struct list_t* list){
	return list->size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
 * lista, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char** list_get_keys(struct list_t* list){
	char** keys = malloc((list->size + 1) * sizeof(char*));
	struct node_t* node = list->head;
	int index = 0;
	while(node->next != NULL){
		keys[index] = malloc(strlen(node->entry->key)+1);
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
int list_free_keys(char** keys){
	// TODO como ver os erros aqui????
	int rows = (sizeof(keys) / sizeof(*keys));
	for (int i = 0; i < rows; i++) {
		free(keys[i]);
	}
	free(keys);

	return 0;
}
