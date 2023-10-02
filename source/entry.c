// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entry.h"
#include "data-private.h"
#include "entry-private.h"
#include "data.h"

struct entry_t* entry_create(char* key, struct data_t* data) {
	if (key == NULL) {
		return NULL;
	}

	if (data_valid(data) == -1) {
		return NULL;
	}

	struct entry_t* entry = malloc(sizeof(struct entry_t));
	if (entry == NULL) {
		return NULL;
	}
	entry->key = key;
	entry->value = data;
	return entry;
}

int entry_destroy(struct entry_t* entry) {
	if (entry_valid(entry) == -1) {
		return -1;
	}

	data_destroy(entry->value);
	free(entry->key);
	free(entry);
	return 0;
}

struct entry_t* entry_dup(struct entry_t* entry) {
	if (entry_valid(entry) == -1) {
		return NULL;
	}

	struct entry_t* new_entry = malloc(sizeof(struct entry_t));
	if (new_entry == NULL) {
		return NULL;
	}

	new_entry->key = malloc(strlen(entry->key) + 1);
	if (new_entry->key == NULL) {
		free(new_entry);
		return NULL;
	}
	strcpy(new_entry->key, entry->key);

	new_entry->value = data_dup(entry->value);
	if (new_entry->value == NULL) {
		free(new_entry->key);
		free(new_entry);
		return NULL;
	}

	return new_entry;
}

int entry_replace(struct entry_t* entry, char* new_key, struct data_t* new_value) {
	if (entry_valid(entry) == -1) {
		return -1;
	}

	if (new_key == NULL) {
		return -1;
	}

	if (data_valid(new_value) == -1) {
		return -1;
	}

	if (data_destroy(entry->value) == -1) {
		return -1;
	}
	free(entry->key);	 // This comes after data_destroy() to avoid the size effect of deleting the entry's key in case data_destroy() fails

	entry->key = new_key;
	entry->value = new_value;

	return 0;
}

int entry_compare(struct entry_t* entry1, struct entry_t* entry2) {
	if (entry_valid(entry1) == -1) {
		return -2;
	}

	if (entry_valid(entry2) == -1) {
		return -2;
	}

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
