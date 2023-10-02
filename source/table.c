// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data-private.h"
#include "list-private.h"
#include "table-private.h"

struct table_t* table_create(int n) {
	if (n <= 0) {
		return NULL;
	}

	struct table_t* table = (struct table_t*)malloc(sizeof(struct table_t));
	if (table == NULL) {
		return NULL;
	}

	table->size = 0;
	table->rows = n;
	table->lists = (struct list_t**)malloc(n * sizeof(struct list_t*));
	if (table->lists == NULL) {
		free(table);
		return NULL;
	}

	// Inicializar as listas
	for (int i = 0; i < table->rows; i++) {
		table->lists[i] = list_create();
		if (table->lists[i] == NULL) {
			for (int j = 0; j < i; j++) {
				list_destroy(table->lists[j]);
			}
			free(table->lists);
			free(table);
			return NULL;
		}
	}

	return table;
}

int table_destroy(struct table_t* table) {
	if (table == NULL) {
		return -1;
	}

	for (int i = 0; i < table->rows; i++) {
		list_destroy(table->lists[i]);
	}
	free(table->lists);
	free(table);

	return 0;
}

int table_put(struct table_t* table, char* key, struct data_t* value) {
	if (table == NULL) {
		return -1;
	}

	if (key == NULL) {
		return -1;
	}

	if (data_valid(value) == -1) {
		return -1;
	}

	char* key_copy = strdup(key);
	if (key_copy == NULL) {
		return -1;
	}

	struct data_t* value_copy = data_dup(value);
	if (value_copy == NULL) {
		free(key_copy);
		return -1;
	}

	struct entry_t* new_entry = entry_create(key_copy, value_copy);
	if (new_entry == NULL) {
		free(key_copy);
		data_destroy(value_copy);
		return -1;
	}

	int row = hash_code(key, table->rows);
	struct list_t* list = table->lists[row];
	int result = list_add(list, new_entry);
	if (result == -1) {
		entry_destroy(new_entry);
		return -1;
	} else if (result == 0) {
		table->size++;
	}

	return 0;
}

struct data_t* table_get(struct table_t* table, char* key) {
	if (table == NULL) {
		return NULL;
	}
	if (key == NULL) {
		return NULL;
	}

	if (table->size == 0) {
		return NULL;
	}

	int row = hash_code(key, table->rows);
	struct entry_t* entry = list_get(table->lists[row], key);
	if (entry == NULL) {
		return NULL;
	}
	return data_dup(entry->value);
}

int table_remove(struct table_t* table, char* key) {
	if (table == NULL) {
		return -1;
	}
	if (key == NULL) {
		return -1;
	}

	if (table->size == 0) {
		return 1;
	}

	int row = hash_code(key, table->rows);
	int result = list_remove(table->lists[row], key);
	if (result == -1) {
		return -1;
	}
	if (result == 0) {
		table->size--;
		return 0;
	}
	return 1;
}

int table_size(struct table_t* table) {
	if (table == NULL) {
		return -1;
	}
	return table->size;
}

char** table_get_keys(struct table_t* table) {
	if (table == NULL) {
		return NULL;
	}

	if (table->size == 0) {
		return NULL;
	}

	char** keys = (char**)calloc(table_size(table) + 1, sizeof(char*));
	if (keys == NULL) {
		return NULL;
	}

	int keyIndex = 0;
	for (int row = 0; row < table->rows; row++) {
		struct list_t* list = table->lists[row];

		char** listKeys = list_get_keys(list);
		int listIndex = 0;
		char* key = listKeys[listIndex];
		while (key != NULL) {
			char* key_copy = (char*)malloc(strlen(key) + 1);
			strcpy(key_copy, key);
			keys[keyIndex++] = key_copy;
			key = listKeys[++listIndex];
		}
		list_free_keys(listKeys);
	}

	return keys;
}

int table_free_keys(char** keys) {
	//return list_free_keys(keys); POR ACASO o codigo é o mesmo por isso podemos chamar esta funcao
	if (keys == NULL) {
		return -1;
	}

	int index = 0;
	char* key = keys[index];
	while (key != NULL) {
		free(key);
		key = keys[++index];
	}
	free(keys);

	return 0;
}

int hash_code(char* key, int module) {
	int index = 0;
	int sum = 0;
	while (key[index] != '\0') {
		sum += key[index];
		index++;
	}
	return sum % module;
}
