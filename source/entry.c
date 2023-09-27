#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entry.h"
#include "data-private.h"
#include "entry-private.h"
#include "data.h"

struct entry_t* entry_create(char* key, struct data_t* data) {
	// Verificar se o key e o data são válidos.
	if (key == NULL) {
		return NULL;
	}

	if (data_valid(data) == -1) {
		return NULL;
	}

	// Alocar memória ao entry.
	struct entry_t* entry = malloc(sizeof(struct entry_t));
	if (entry == NULL) {
		return NULL;
	}
	// Alocar memória para a key e atribuir a key à entry:
    entry->key = malloc(strlen(entry->key) + 1);
	if (entry->key == NULL) {
		free(entry);
		return NULL;
	}
	strcpy(entry->key, entry->key);
	// Alocar memória para a data e atribuir a data à entry.
	entry->value = data_dup(data);
	if(entry->value == NULL) {
		free(entry->key);
        free(entry);
        return NULL;
	}

	return entry;
}

int entry_destroy(struct entry_t* entry) {
	// Verificar se a entry é válida.
	if (entry_valid(entry) == -1) {
		return -1;
	}

	// Libertar memória alocada pela entry e os seus atributos.
	data_destroy(entry->value);
	free(entry->key);
	free(entry);
	return 0;
}

struct entry_t* entry_dup(struct entry_t* entry) {
	// Verificar se a entry é válida.
	if (entry_valid(entry) == -1) {
		return NULL;
	}

	// Alocar memória à nova entry.
	struct entry_t* new_entry = malloc(sizeof(struct entry_t));
	if (new_entry == NULL) {
		return NULL;
	}

	// Alocar memória à key da nova entry e atribuí-la à nova entry.
	new_entry->key = malloc(strlen(entry->key) + 1);
	if (new_entry->key == NULL) {
		free(new_entry);
		return NULL;
	}
	strcpy(new_entry->key, entry->key);

	// Alocar memória à data da nova entry e atribuí-la à nova entry.
	new_entry->value = data_dup(entry->value);
	if (new_entry->value == NULL) {
		free(new_entry->key);
		free(new_entry);
		return NULL;
	}

	return new_entry;
}

int entry_replace(struct entry_t* entry, char* new_key, struct data_t* new_value) {
	// Verificar se a entry é válida.
	if (entry_valid(entry) == -1) {
		return -1;
	}

	// Verificar se a key é válida.
	if (new_key == NULL) {
		return -1;
	}

	// Destruir a data antiga, o data_destroy já verifica se a data é válida.
	if (data_destroy(entry->value) == -1) {
		return -1;
	}
	// Destruir a key antiga.
	free(entry->key);	 // This comes after data_destroy() to avoid the size effect of deleting the entry's key in case data_destroy() fails

	entry->key = new_key;
	entry->value = new_value;

	return 0;
}

int entry_compare(struct entry_t* entry1, struct entry_t* entry2) {
	// Verificar se a entry1 é válida.
	if (entry_valid(entry1) == -1) {
		return -2;
	}

	// Verificar se a entry2 é válida.
	if (entry_valid(entry2) == -1) {
		return -2;
	}

	// Comparar as keys das entrys.
	int compare = strcmp(entry1->key, entry2->key);

	if (compare < 0) {
		return -1;
	}

	if (compare > 0) {
		return 1;
	}

	return 0;
}

int entry_valid(struct entry_t* entry) {
	if (entry == NULL) {
		return -1;
	}

	if (entry->key == NULL) {
		return -1;
	}

	if (data_valid(entry->value) == -1) {
		return -1;
	}

	return 0;
}
