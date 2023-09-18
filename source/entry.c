#include <stdio.h>
#include <entry.h>
#include <string.h>


/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados de entrada.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t* entry_create(char* key, struct data_t* data){
	struct entry_t* entry = malloc(sizeof(struct entry_t));
	if(entry ==NULL){
		return NULL;
	}
	entry->key = key;
	//entry->value = data_dup(data); ficara assim??
	entry->value = data;
	return entry;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_destroy(struct entry_t* entry){
	data_destroy(entry->value);
	free(entry->key);
	free(entry);
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t* entry_dup(struct entry_t* entry){
	struct entry_t* new_entry = malloc(sizeof(struct entry_t));

	if (new_entry == NULL){
		return NULL;
	}

	new_entry->value = data_dup(entry->value);
	strcpy(new_entry->key, entry->key);
	entry_destroy(entry);
	return new_entry;
}

/* Função que substitui o conteúdo de uma entry, usando a nova chave e
 * o novo valor passados como argumentos, e eliminando a memória ocupada
 * pelos conteúdos antigos da mesma.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_replace(struct entry_t* entry, char* new_key, struct data_t* new_value){

	if (entry == NULL || new_key == NULL || new_value == NULL) {
		return -1;
	}

	data_destroy(entry->value);
	free(entry->key);
	entry->key = new_key;
	entry->value = new_value;

	return 0;
}

/* Função que compara duas entries e retorna a ordem das mesmas, sendo esta
 * ordem definida pela ordem das suas chaves.
 * Retorna 0 se as chaves forem iguais, -1 se entry1 < entry2,
 * 1 se entry1 > entry2 ou -2 em caso de erro.
 */
int entry_compare(struct entry_t* entry1, struct entry_t* entry2){
	return strcmp(entry1->key, entry2->key);
}
