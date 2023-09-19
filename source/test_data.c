#include <data.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Função auxiliar para sair do programa com uma dada msg de erro.
// Não sei se ainda vai ser necessária.
void error_exit(const char* msg) {
	perror(msg);
	exit(0);
}

/**************************************************************/
/* Testa a criação do módulo data. Se este teste for efetuado
 * com sucesso, o valor do result será: 6
 */
int test_data_create() {
	printf("Módulo data -> teste_data_create\n");

	int result = 0;

	result += (data_create(0, "a") == NULL);
	result += (data_create(1, "a") != NULL);

	char* string_test = "test";
	result += (data_create(0, string_test) == NULL);
	result += (data_create(10, string_test) != NULL);

	result += (data_create(0, NULL) == NULL);
	result += (data_create(5, NULL) == NULL);

	printf("data - test_data_create: ");
	if (result == 6) {
		printf("passou!\n");
	} else if (result == 0) {
		printf("falhou completamente.\n");
	} else {
		printf("falhou em certos testes.\n");
	}
	return result;
}

/**************************************************************/
/* Testa a destruição do módulo data. Se este teste for
 * efetuado com sucesso, o valor do result será: 2
 */
int test_data_destroy() {
	printf("Módulo data -> test_data_destroy\n");

	int result = 0;
	struct data_t* good_data = data_create(15, strdup("good"));
	struct data_t* bad_data = NULL;  // esta é má porque não alocamos memória!
	// bad_data->datasize = 10;
	// bad_data->data = "evil";

	result += (data_destroy(good_data) == 0);
	result += (data_destroy(bad_data) == -1);

	printf("data - test_data_destroy: ");
	if (result == 2) {
		printf("passou!\n");
	} else if (result == 0) {
		printf("falhou completamente.\n");
	} else {
		printf("falhou em certos testes.\n");
	}
	return result;
}

/**************************************************************/
/* Testa a duplicação do módulo data. E se a data duplicada
 * consegue fazer outras operações. Se este teste for
 * efetuado com sucesso, o valor do result será: 6
 */
int test_data_dup() {
	printf("Módulo data -> test_data_dup\n");

	int result = 0;
	struct data_t* good_data = data_create(27, strdup("good"));
	struct data_t* bad_data = NULL;  // esta é má porque não alocamos memória!
	// bad_data->datasize = 31;
	// bad_data->data = "evil";

	result += (data_dup(good_data) != NULL);
	result += (data_dup(bad_data) == NULL);

	struct data_t* dup_good_data = data_dup(good_data);
	result += (data_dup(dup_good_data) != NULL);

	result += (data_replace(dup_good_data, 10, strdup("good dup")) == 0);
	result += (data_dup(dup_good_data) != NULL);

	result += (data_destroy(dup_good_data) == 0);

	printf("data - test_data_dup: ");
	if (result == 6) {
		printf("passou!\n");
	} else if (result == 0) {
		printf("falhou completamente.\n");
	} else {
		printf("falhou em certos testes.\n");
	}
	return result;
}

/**************************************************************/
/* Testa a substituição do módulo data. Se este teste for
 * efetuado com sucesso, o valor do result será: 4
 */
int test_data_replace() {
	printf("Módulo data -> test_data_replace\n");

	int result = 0;
	struct data_t* good_data = data_create(50, strdup("good"));
	struct data_t* bad_data = NULL;  // esta é má porque não alocamos memória!
	// bad_data->datasize = 36;
	// bad_data->data = "evil";

	result += (data_replace(good_data, 31, "evil") == 0);
	result += (data_replace(bad_data, 1, "good") == -1);

	result += (good_data->datasize == 31);
	result += (memcmp(good_data->data, "evil", 5) == 0);	 // se os dois estiverem bem então duplicou bem.

	printf("data - data_replace: ");
	if (result == 4) {
		printf("passou!\n");
	} else if (result == 0) {
		printf("falhou completamente.\n");
	} else {
		printf("falhou em certos testes.\n");
	}
	return result;
}

/**************************************************************/
int main() {
	int result = 0;
	result += test_data_create();
	printf("\n");
	result += test_data_destroy();
	printf("\n");
	result += test_data_dup();
	printf("\n");
	result += test_data_replace();
	printf("\n");

	if (result == 0) {
		printf("Todos os testes falharam.\n");
	} else if (result == 18) {
		printf("Passou todos os testes!\n");
	} else {
		printf("Falhou certos testes.\n");
	}
}
