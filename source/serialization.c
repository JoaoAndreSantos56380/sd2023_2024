// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serialization.h"

int keyArray_to_buffer(char** keys, char** keys_buf) {
	// Verificar se o array de keys é válido.
	if (keys == NULL || keys_buf == NULL) {
		return -1;
	}

	// Obter o nº de keys (nkeys) no array de keys e calcular o tamanho do keys_buf (keys_buf_size).
	int nkeys = 0;
	int keys_buf_size = sizeof(int);	 // tamanho inicial serve para guardar o nº de chaves (nkeys) no keys_buf.
	while (keys[nkeys]) {
		keys_buf_size += strlen(keys[nkeys]) + 1;	 //+1 para o char nulo no final '\0'.
		nkeys++;
	}

	// Alocar memória no keys_buf.
	*keys_buf = (char*)malloc(keys_buf_size);
	if (*keys_buf == NULL) {
		return -1;	// Falha ao alocar memória no keys_buf!
	}

	// Copiar o nº de keys (nkeys) para o início do keys_buf. Transformando o nº de keys em network byte order!
	int nkeys_network_byte_order = htonl(nkeys);
	memcpy(*keys_buf, &nkeys_network_byte_order, sizeof(int));
	char* keys_buf_ptr = *keys_buf + sizeof(int);			// Pointer que aponta para a posição atual no buffer, neste caso, seguindo o exemplo acima, estaria em 'key1'.

	// Copiar as chaves para o keys_buf.
	for (int i = 0; i < nkeys; i++) {
		strcpy(keys_buf_ptr, keys[i]);		  // copia o keys[i] para a posição atual do ponteiro.
		keys_buf_ptr += strlen(keys[i]) + 1;  // incrementar para colocar as keys seguintes.
	}

	return keys_buf_size;  // retorna o tamanho alocado no buffer em caso de sucesso.
}

char** buffer_to_keyArray(char* keys_buf) {
	// Verificar se o keys_buf é válido.
	if (keys_buf == NULL) {
		return NULL;
	}

	// Ler o nº de chaves (nkeys) do keys_buf. O num_keys deve ser transformado de network byte order para host byte order!
	int nkeys_network_byte_order;
	memcpy(&nkeys_network_byte_order, keys_buf, sizeof(int));
	int nkeys = ntohl(nkeys_network_byte_order);

	// Mesmo processo do ponteiro para ler as keys no keys_buf.
	char* keys_buf_ptr = keys_buf + sizeof(int);

	// Alocar memória no array de strings.
	char** keys = (char**)malloc((nkeys + 1) * sizeof(char*));	//+1 para o char nulo no final '\0'.
	if (keys == NULL) {
		return NULL;  // Falha ao alocar memória nas keys!
	}

	// Copiar as chaves de keys_buf para o array keys.
	for (int i = 0; i < nkeys; i++) {
		keys[i] = strdup(keys_buf_ptr);	// strdup aloca memória para a nova string/key e duplica.

		if (keys[i] == NULL) {	// caso haja erro a alocar memória para alguma key.
			for (int j = 0; j < i; j++) {
				free(keys[j]);
			}
			free(keys);
			return NULL;  // pois ocorreu erro a alocar memória a alguma key.
		}
		keys_buf_ptr += strlen(keys_buf_ptr) + 1;	 // Avança para a próxima key. Pois strlen devolve o tamanho da string + char nulo.
	}
	return keys;
}
