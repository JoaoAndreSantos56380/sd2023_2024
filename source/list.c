// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		if (compare == 1) {
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
		if (compare == -1) {
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
		int compare = entry_compare(entry, current->entry);
		// We've passed the right node. Use the previous.
		if (compare == -1) {
			struct node_t* new_node = (struct node_t*)malloc(sizeof(struct node_t));
			if (new_node == NULL) {
				return -1;
			}
			new_node->entry = entry;
			new_node->next = current;
			if (previous == NULL) {
				list->head = new_node;
			} else {
				previous->next = new_node;
			}

			list->size++;
			return 0;
		}
		// We're on the right node. Replace.
		if (compare == 0) {
			entry_destroy(current->entry);
			current->entry = entry;
			return 1;
		}
		// We're on the last node. Append.
		if (next == NULL) {
			struct node_t* new_node = (struct node_t*)malloc(sizeof(struct node_t));
			if (new_node == NULL) {
				return -1;
			}
			new_node->entry = entry;
			new_node->next = NULL;
			current->next = new_node;
			list->size++;
			return 0;
		}
		previous = current;
		current = next;
	}

	return -1;
}

int list_remove(struct list_t* list, char* key) {
	if (valid_list(list) == -1) {
		return -1;
	}

	if (key == NULL) {
		return -1;
	}

	if (list->size == 0) {
		return 1;
	}

	struct node_t* current = list->head;
	struct node_t* previous = NULL;
	while (current != NULL) {
		struct node_t* next = current->next;
		int compare = strcmp(key, current->entry->key);
		// We're on the right node. Delete.
		if (compare == 0) {
			// It's the first node. Update list->head.
			if (previous == NULL) {
				list->head = current->next;
			} else {	 // It's not the first node. Update the previous.
				previous->next = current->next;
			}
			entry_destroy(current->entry);
			free(current);
			list->size--;
			return 0;
		}
		previous = current;
		current = next;
	}

	return 1;
}

struct entry_t* list_get(struct list_t* list, char* key) {
	if (valid_list(list) == -1) {
		return NULL;
	}

	if (key == NULL) {
		return NULL;
	}

	if (list->size == 0) {
		return NULL;
	}

	struct node_t* current = list->head;
	while (current != NULL) {
		if (strcmp(key, current->entry->key) == 0) {
			return current->entry;
		}
		current = current->next;
	}
	return NULL;
}

int list_size(struct list_t* list) {
	if (valid_list(list) == -1) {
		return -1;
	}
	return list->size;
}

char** list_get_keys(struct list_t* list) {
	if (valid_list(list) == -1) {
		return NULL;
	}

	if (list->size == 0) {
		return NULL;
	}

	char** keys = (char**)calloc((list->size + 1), sizeof(char*));
	struct node_t* current = list->head;
	int index = 0;
	while (current != NULL) {
		char* str = (char*)malloc(strlen(current->entry->key) + 1);
		strcpy(str, current->entry->key);
		keys[index++] = str;
		current = current->next;
	}
	return keys;
}

int list_free_keys(char** keys) {
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

int valid_list(struct list_t* list) {
	if (list == NULL) {
		return -1;
	}

	if (list->head != NULL && list->size < 0) {
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

void print_list() {
}
