#include <data.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <data-private.h>

struct data_t* data_create(int size, void* data) {
	if (size <= 0) {
		return NULL;
	}

	if (data == NULL) {
		return NULL;
	}

	struct data_t* new_data = malloc(sizeof(struct data_t));

	if (new_data == NULL) {
		return NULL;
	}

	new_data->datasize = size;
	new_data->data = data;

	return new_data;
}

int data_destroy(struct data_t* data) {
	if (data_valid(data) == -1) {
		return -1;
	}

	free(data->data);
	free(data);
	return 0;
}

struct data_t* data_dup(struct data_t* data) {
	if (data_valid(data) == -1) {
		return NULL;
	}

	struct data_t* new_data = malloc(sizeof(struct data_t));

	if (new_data == NULL) {
		return NULL;
	}

	new_data->datasize = data->datasize;
	new_data->data = malloc(new_data->datasize);
	if (new_data->data == NULL) {
		free(new_data);
		return NULL;
	}
	memcpy(new_data->data, data->data, new_data->datasize);

	return new_data;
}

int data_replace(struct data_t* data, int new_size, void* new_data) {
	if (data_valid(data) == -1) {
		return -1;
	}

	if (new_size <= 0) {
		return -1;
	}

	if (new_data == NULL) {
		return -1;
	}

	free(data->data);
	data->datasize = new_size;
	data->data = new_data;
	return 0;
}

int data_valid(struct data_t* data) {
	if (data == NULL) {
		return -1;
	}

	if (data->data == NULL) {
		return -1;
	}

	if (data->datasize <= 0) {
		return -1;
	}

	return 0;
}
