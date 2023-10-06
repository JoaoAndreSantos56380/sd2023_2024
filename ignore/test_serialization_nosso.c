#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serialization.h"

/**************************************************************/
/*
 * Testa a serialização. Se este teste for efetuado com sucesso,
 * o programa imprime "passou!".
 */
void test_keyArray_to_buffer() {
	int num_keys = 5;	 // número de keys no array

	char** good_keys_array = (char**)malloc(num_keys * sizeof(char*));  // alocar memória para o array
	if (good_keys_array == NULL) {
		perror("Falha na alocação de memória para o good array de keys..");
	}

	// Alocar memória para cada elemento do array.
	for (int i = 0; i < num_keys; i++) {
		good_keys_array[i] = (char*)malloc(101 * sizeof(char));	// Alocamos espaço para uma string de até 100 caracteres, mas podia ter mais ou menos!
		if (good_keys_array[i] == NULL) {
			perror("Falha na alocação de memória para o elemento do good array de keys..");
		}
	}
	strcpy(good_keys_array[0], "keY boa");
	strcpy(good_keys_array[1], "kEY MALVAda");
	strcpy(good_keys_array[2], "kEy BONITA");
	strcpy(good_keys_array[3], "key");
	strcpy(good_keys_array[4], "ABCdefG rip bozo");

	char** good_keys_buf;
	assert(keyArray_to_buffer(good_keys_array, good_keys_buf) == num_keys);

	int buffer_num_keys;
	memcpy(&buffer_num_keys, *good_keys_buf, sizeof(int));
	char* good_keys_buf_ptr = *good_keys_buf + sizeof(int);
	for (int i = 0; i < num_keys; i++) {
		assert(strcmp(good_keys_buf_ptr, good_keys_array[i]) == 0);	 // verifica se as keys estão corretas
		good_keys_buf_ptr += strlen(good_keys_buf_ptr) + 1;
	}

	char** bad_keys_buf = malloc(sizeof(int));
	assert(keyArray_to_buffer(good_keys_array, bad_keys_buf) == -1);	// testa um buffer errado.

	printf("test_keyArray_to_buffer: passou!");
}
/**************************************************************/
/*
 * Testa a de-serialização. Se este teste for efetuado com sucesso,
 * o programa imprime "passou!".
 */
void test_buffer_to_keyArray() {
	int num_keys = 5;	 // número de keys no array

	char** good_keys_array = (char**)malloc(num_keys * sizeof(char*));  // alocar memória para o array
	if (good_keys_array == NULL) {
		perror("Falha na alocação de memória para o good array de keys..");
	}

	// Alocar memória para cada elemento do array.
	for (int i = 0; i < num_keys; i++) {
		good_keys_array[i] = (char*)malloc(101 * sizeof(char));	// Alocamos espaço para uma string de até 100 caracteres, mas podia ter mais ou menos!
		if (good_keys_array[i] == NULL) {
			perror("Falha na alocação de memória para o elemento do good array de keys..");
		}
	}
	strcpy(good_keys_array[0], "keY boa");
	strcpy(good_keys_array[1], "kEY MALVAda");
	strcpy(good_keys_array[2], "kEy BONITA");
	strcpy(good_keys_array[3], "key");
	strcpy(good_keys_array[4], "ABCdefG rip bozo");

	char** good_keys_buf;
	keyArray_to_buffer(good_keys_array, good_keys_buf);

	char** result = buffer_to_keyArray(good_keys_buf);
	for (int i = 0; i < num_keys; i++) {
		assert(strcmp(result[i], good_keys_array[i]) == 0);  // mais uma vez verificar se as keys estão corretas
	}

	char** bad_keys_buf;
	assert(buffer_to_keyArray(bad_keys_buf) == NULL);

	printf("test_buffer_to_keyArray: passou!");
}

int main() {
	test_keyArray_to_buffer();
	printf("\n");
	test_buffer_to_keyArray();
	printf("\n");
	return 0;
}
