#include <data.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Função que cria um novo elemento de dados data_t e que inicializa
 * os dados de acordo com os argumentos recebidos, sem necessidade de
 * reservar memória para os dados.
 * Retorna a nova estrutura ou NULL em caso de erro.
 */
struct data_t* data_create(int size, void* data){
	if(size <= 0){
		return NULL;
	}

	struct data_t* new_data = malloc(sizeof(struct data_t));

	if (data == NULL){
		return NULL;
	}

	new_data->datasize = size;
	new_data->data = data;

	return new_data;

}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int data_destroy(struct data_t* data){
	if (data != NULL) {
		if (data->data != NULL){
			free(data->data);
		}
		free(data);
		return 0;
	}
	return -1;
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 * Retorna a nova estrutura ou NULL em caso de erro.
 */
struct data_t* data_dup(struct data_t* data){
	struct data_t* new_data = malloc(sizeof(struct data_t));//data_create(data->datasize, data->data);

	if (new_data == NULL || data == NULL) {
		return NULL;
	}

	new_data->datasize = data->datasize;
	new_data->data = malloc(new_data->datasize);//data->data;

	memcpy(new_data->data, data->data, new_data->datasize);

	//data_destroy(data);

	return new_data;
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
 * Deve assegurar que liberta o espaço ocupado pelo conteúdo antigo.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int data_replace(struct data_t* data, int new_size, void* new_data){
	if (data == NULL) {
		return -1;
	}

	if (data->data != NULL) {
		free(data->data);
	}
	// TODO ver como fazer o caso de erro
	data->datasize = new_size;
	data->data = new_data;
	return 0;
}
