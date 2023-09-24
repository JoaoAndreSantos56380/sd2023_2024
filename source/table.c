#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "table-private.h"
#include "list-private.h"

int module; // TODO posso ter esta variavel global?

struct table_t* table_create(int n) {
	// Verificar se o n não é negativo.
	if(n <= 0) {
    	return NULL;
    }

	// Alocar memória à table e aos respetivos atributos.
	struct table_t* table = malloc(sizeof(struct table_t));
	if(table == NULL) {
    	return NULL;
    }
	table->lists = malloc(n * sizeof(struct list_t*));
	if(table->lists == NULL) {
		free(table);
        return NULL; 
	}
	table->size = 0;

	// Criar as listas.
	for (int i = 0; i < n; i++) {
        table->lists[i] = list_create();

		// Falha ao criar de uma lista, dar free da memória alocada às listas anteriores, às lists e à table.
        if(table->lists[i] == NULL) {
            for (int j = 0; j < i; j++) {
                list_destroy(table->lists[j]);
            }
            free(table->lists);
            free(table);

            return NULL;
        }
    }

	//module = n;
	return table;
}

int table_destroy(struct table_t* table) {
	// Verificar se a table é válida.
	if(table == NULL) {
		return -1;
	}

	// Destruir as listas da table e dar free do lists e da table.
	for (size_t i = 0; i < table->size; i++) {
		list_destroy(table->lists[i]);
	}
	free(table->lists);
	free(table);

	return 0;
}

int table_put(struct table_t* table, char* key, struct data_t* value) {
	// Verificar se os argumentos passados são válidos.
	if(table == NULL) {
		return -1;
	}
	if(key == NULL) {
		return -1;
	}
	if(data_valid(value) == -1) {
		return -1;
	}
	
	int hash = hash_code(key, table->size);

	// Fazer cópias da key e do value.
	char *key_copy = strdup(key);
    if(key_copy == NULL) {
        return -1;
    }

    struct data_t *value_copy = data_dup(value);
    if(value_copy == NULL) {
        free(key_copy);
        return -1;
    }

	struct entry_t *new_entry = entry_create(key_copy, value_copy);
	int result = list_add(table->lists[hash], new_entry);

	// Libertar espaço alocado das cópias e da nova entry. Podiamos simplesmente fazer entry_destroy??
	free(key_copy);
	data_destroy(value_copy);
	entry_destroy(new_entry);

	return (result >= 0) ? 0 : -1;
}

struct data_t* table_get(struct table_t* table, char* key) {
	// Verificar se os argumentos passados são válidos.
	if(table == NULL) {
		return NULL;
	}
	if(key == NULL) {
		return NULL;
	}

	int hash = hash_code(key, table->size);
	struct entry_t* entry = list_get(table->lists[hash], key);
	if (entry == NULL) { // Se a key não for encontrada.
    	return NULL;
    }
	return data_dup(entry->value);
}

int table_remove(struct table_t* table, char* key) {
	// Verificar se os argumentos são válidos.
	if(table == NULL) {
		return NULL;
	}
	if(key == NULL) {
		return NULL;
	}

	int hash = hash_code(key, table->size);
	return list_remove(table->lists[hash], key);
}

int table_size(struct table_t* table){
	return table->size;
}

char** table_get_keys(struct table_t* table) {
	// Verificar se a table é válida.
	if(table == NULL) {
		return NULL;
	}

	char** keys = malloc((table_size(table) + 1) * sizeof(char*));
	// Verificar se a memória foi alocada com sucesso.
	if(keys == NULL) {
    	return NULL;
    }

	char **current_key = keys; // Ponteiro para a posição atual do array de keys.
	for(int i = 0; i < table->size; i++) {
		struct list_t *current_list = table->lists[i];

		// Iterar sobre os nodes na lista.
        struct node_t *current_node = current_list->head;
        while(current_node != NULL) {
            // Copiar a chave para o array de strings.
            *current_key = strdup(current_node->entry->key);
            if(*current_key == NULL) {
                // Falha ao alocar memória para a key. Libertar toda a memória alocada até agora.
                char **key_to_free = keys; // Novo ponteiro para ir dando free à memória das keys.
                while(key_to_free < current_key) {
                    free(*key_to_free);
                    key_to_free++;
                }
                free(keys);
                return NULL;
            }

            current_key++;
            current_node = current_node->next;
		}
	}
	return keys;
}

int table_free_keys(char** keys) {
	// Verificar se as keys são válidas.
	if(keys == NULL) {
		return -1;
	}
	
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
