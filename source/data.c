#include <data.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <data-private.h>

struct data_t* data_create(int size, void* data) {

	//verficar se o size e o data sao validos
	if (size <= 0) {
		return NULL;
	}

	if (data == NULL) {
		return NULL;
	}

	// alocar memoria para a estrutura
	struct data_t* new_data = malloc(sizeof(struct data_t));

	// se a memoria nao for alocada retornar erro
	if (new_data == NULL) {
		return NULL;
	}

	// atribuir o size e o data a nova estrutura criada
	new_data->datasize = size;
	new_data->data = data;

	return new_data;
}

int data_destroy(struct data_t* data) {

	// verificar se a estrutura data e valida
	if (data_valid(data) == -1) {
		return -1;
	}

	// lbertar memoria da estrutura
	free(data->data);
	free(data);
	return 0;
}

struct data_t* data_dup(struct data_t* data) {

	// verificar se a estrutura data e valida
	if (data_valid(data) == -1) {
		return NULL;
	}

	// alocar memoria para a estrutura nova
	struct data_t* new_data = malloc(sizeof(struct data_t));

	// verificar se a memoria foi alocada
	if (new_data == NULL) {
		return NULL;
	}

	// passar os atributos datasize e data para o duplicado
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

	// verificar se a estrutura data e os novos atributos sao validos
	if (data_valid(data) == -1) {
		return -1;
	}

	if (new_size <= 0) {
		return -1;
	}

	if (new_data == NULL) {
		return -1;
	}

	// libertar a memoria usada pela estrutura anteorior
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
