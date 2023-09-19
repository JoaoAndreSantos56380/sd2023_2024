#include <stdio.h>
#include <stdlib.h>
#include <table-private.h>
#include <list-private.h>
/* Função para criar e inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash).
 * Retorna a tabela ou NULL em caso de erro.
 */
int module; // TODO posso ter esta variavel global?
struct table_t* table_create(int n){
	struct table_t* table = malloc(sizeof(struct table_t));
	table->lists = malloc(n * sizeof(struct list_t*));
	table->size = 0;
	//module = n;
}

/* Função que elimina uma tabela, libertando *toda* a memória utilizada
 * pela tabela.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_destroy(struct table_t* table){
	for (size_t i = 0; i < (sizeof(table->lists) / sizeof(*table->lists)); i++) { // TODO como e que se explica esta divisao?
		list_destroy(table->lists[i]);
	}
	free(table);
}

/* Função para adicionar um par chave-valor à tabela. Os dados de entrada
 * desta função deverão ser copiados, ou seja, a função vai criar uma nova
 * entry com *CÓPIAS* da key (string) e dos dados. Se a key já existir na
 * tabela, a função tem de substituir a entry existente na tabela pela
 * nova, fazendo a necessária gestão da memória.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t* table, char* key, struct data_t* value){
	// TODO como e que fica o put???
	int hash = hash_code(key, module);
	struct entry_t* new_entry = entry_create(key, value);
	int result = list_add(table->lists[hash], new_entry);
	result >= 0 ? 0 : -1;
}

/* Função que procura na tabela uma entry com a chave key.
 * Retorna uma *CÓPIA* dos dados (estrutura data_t) nessa entry ou
 * NULL se não encontrar a entry ou em caso de erro.
 */
struct data_t* table_get(struct table_t* table, char* key){
	int hash = hash_code(key, module);
	struct entry_t* entry = list_get(table->lists[hash], key);
	return data_dup(entry->value);
}

/* Função que remove da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int table_remove(struct table_t* table, char* key){
	int hash = hash_code(key, module);
	return list_remove(table->lists[hash], key);
}

/* Função que conta o número de entries na tabela passada como argumento.
 * Retorna o tamanho da tabela ou -1 em caso de erro.
 */
int table_size(struct table_t* table){
	return table->size;
}

/* Função que constrói um array de char* com a cópia de todas as keys na
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
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

/* Função que liberta a memória ocupada pelo array de keys obtido pela
 * função table_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_free_keys(char** keys){
	// TODO como ver os erros aqui????
	int rows = (sizeof(keys) / sizeof(*keys));
	for (int i = 0; i < rows; i++) {
		free(keys[i]);
	}
	free(keys);

	return 0;
}

/* Função que calcula o índice da lista a partir da chave
 */
int hash_code(char* key, int n){
	int index = 0;
	int sum = 0;
	while (key[index] != '\0') {
		sum += key[index];
		index++;
	}
	return sum % n;
}
