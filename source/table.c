#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "table-private.h"
#include "list-private.h"

int module; // TODO posso ter esta variavel global?
struct table_t* table_create(int n){
	struct table_t* table = malloc(sizeof(struct table_t));
	table->lists = malloc(n * sizeof(struct list_t*));
	table->size = 0;
	//module = n;
}

int table_destroy(struct table_t* table){
	for (size_t i = 0; i < (sizeof(table->lists) / sizeof(*table->lists)); i++) { // TODO como e que se explica esta divisao?
		list_destroy(table->lists[i]);
	}
	free(table);
}

int table_put(struct table_t* table, char* key, struct data_t* value){
	// TODO como e que fica o put???
	int hash = hash_code(key, module);
	struct entry_t* new_entry = entry_create(key, value);
	int result = list_add(table->lists[hash], new_entry);
	result >= 0 ? 0 : -1;
}

struct data_t* table_get(struct table_t* table, char* key){
	int hash = hash_code(key, module);
	struct entry_t* entry = list_get(table->lists[hash], key);
	return data_dup(entry->value);
}

int table_remove(struct table_t* table, char* key){
	int hash = hash_code(key, module);
	return list_remove(table->lists[hash], key);
}

int table_size(struct table_t* table){
	return table->size;
}

char** table_get_keys(struct table_t* table){
	char** keys = malloc((table_size(table) + 1) * sizeof(char*));
	for (int i = 0; i < table->size; i++) {
		if (table->lists[i] != NULL && table->lists[i]->head != NULL) {
			struct node_t* node = table->lists[i]->head;
			while (node->next != NULL) {
				//fazer memcpy e ver situacao do ponteiro, como e que anda para a frente
			}
		}
	}
	return keys;
}

int table_free_keys(char** keys){
	// TODO como ver os erros aqui????
	int rows = (sizeof(keys) / sizeof(*keys));
	for (int i = 0; i < rows; i++) {
		free(keys[i]);
	}
	free(keys);

	return 0;
}

int hash_code(char* key, int n){
	int index = 0;
	int sum = 0;
	while (key[index] != '\0') {
		sum += key[index];
		index++;
	}
	return sum % n;
}
