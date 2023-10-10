#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "data-private.h"

/**************************************************************/
/* Testa a criação do módulo data. Se este teste for efetuado
 * com sucesso, o programa imprime "passou!".
 */
void test_data_create() {
	assert(data_create(0, "a") == NULL);
	assert(data_create(1, "a") != NULL);

	char* string_test = "test";

	assert(data_create(0, string_test) == NULL);
	assert(data_create(10, string_test) != NULL);

	int test_int = 8;
	assert(data_create(0, &test_int) == NULL);
	assert(data_create(10, &test_int) != NULL);

	// supostamente o data podera ser qualuqer coisa
	//  portanto poderiamos ate passar uma estrutura data para ele proprio

	assert(data_create(0, NULL) == NULL);
	assert(data_create(5, NULL) == NULL);

	printf("test_data_create: passou!");
}

/**************************************************************/
/* Testa a destruição do módulo data. Se este teste for
 * efetuado com sucesso, o programa imprime "passou!".
 */
void test_data_destroy() {
	struct data_t* good_data = data_create(15, strdup("good"));
	struct data_t* bad_data = NULL;
	struct data_t* bad_data_2 = data_create(0, strdup("bad"));

	// double test_double = 3.141592653589793;
	// struct data_t* good_data_2 = data_create(67, &test_double);

	// assert(data_destroy(good_data_2) == 0);
	assert(data_destroy(good_data) == 0);
	assert(data_destroy(bad_data) == -1);
	assert(data_destroy(bad_data_2) == -1);

	printf("test_data_destroy: passou!");
}

/**************************************************************/
/* Testa a duplicação do módulo data. Se este teste for
 * efetuado com sucesso, o programa imprime "passou!".
 */
void test_data_dup() {
	struct data_t* good_data = data_create(27, strdup("good"));
	struct data_t* bad_data = NULL;

	assert(data_dup(good_data) != NULL);
	assert(data_dup(bad_data) == NULL);

	struct data_t* dup_good_data = data_dup(good_data);
	assert(data_dup(dup_good_data) != NULL);

	assert(data_replace(dup_good_data, 10, strdup("good dup")) == 0);
	assert(data_dup(dup_good_data) != NULL);

	assert(data_destroy(dup_good_data) == 0);

	printf("test_data_dup: passou!");
}

/**************************************************************/
/* Testa a substituição do módulo data. Se este teste for
 * efetuado com sucesso, o programa imprime "passou!".
 */
void test_data_replace() {
	struct data_t* good_data = data_create(50, strdup("good"));
	struct data_t* bad_data = NULL;
	struct data_t* bad_data_2 = data_create(0, strdup("bad"));

	assert(data_replace(good_data, 31, "evil") == 0);
	assert(data_replace(bad_data, 1, "good") == -1);
	assert(data_replace(bad_data_2, 16, "good") == -1);

	assert(good_data->datasize == 31);
	assert(memcmp(good_data->data, "evil", 5) == 0);

	printf("test_data_replace: passou!");
}

/**************************************************************/
int main() {
	test_data_create();
	printf("\n");
	test_data_destroy();
	printf("\n");
	test_data_dup();
	printf("\n");
	test_data_replace();
	printf("\n");

	return 0;
}
