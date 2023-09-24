#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/**************************************************************/
/*
 * Testa a criação de uma lista. Se este teste for efetuado
 * com sucesso, o programa imprime "passou!".
 */
void test_list_create() {
	struct list_t* good_list = list_create();
	assert(good_list != NULL);

	printf("test_list_create: passou!");
}

/**************************************************************/
/*
 * Testa a adição de uma entry a uma lista. Se este teste for
 * efetuado com sucesso, o programa imprime "passou!".
 */
void test_list_add() {
	struct list_t* good_list = list_create();
	struct list_t* bad_list;
	struct entry_t* bad_entry;
	bad_entry->key = "chave";
	bad_entry->value->datasize = 10;
	bad_entry->value->data = "errado";

	struct data_t* data1 = data_create(25, 12);
	struct entry_t* entry1 = entry_create("chave", data1);
	struct data_t* data2 = data_create(100, "bruh");
	struct entry_t* entry2 = entry_create("chavona", data2);
	struct data_t* data3 = data_create(5, 0);
	struct entry_t* entry3 = entry_create("chave", data3);  // entry com chave igual à entry1

	assert(list_add(bad_list, entry1) == -1);		  // testa lista incorreta
	assert(list_add(good_list, bad_entry) == -1);  // testa entry incorreta

	assert(list_add(good_list, entry1) == 0);	 // testa se o add está a ser feito corretamente
	assert(list_add(good_list, entry2) == 0);
	assert(list_add(good_list, entry3) == 1);

	printf("test_list_add: passou!");
}

/**************************************************************/
/*
 * Testa a remoção de uma entry da lista. Se este teste for
 * efetuado com sucesso, o programa imprime "passou!".
 */
void test_list_remove() {
	struct list_t* good_list = list_create();
	struct list_t* bad_list;
	struct entry_t* bad_entry;

	struct data_t* data1 = data_create(25, 12);
	struct entry_t* entry1 = entry_create("chave", data1);
	struct data_t* data2 = data_create(100, "bruh");
	struct entry_t* entry2 = entry_create("chavona", data2);

	list_add(good_list, entry1);
	list_add(good_list, entry2);

	assert(list_remove(good_list, "chave") == 0);
	assert(list_remove(good_list, "chave") == 1);
	assert(list_remove(bad_list, "chave") == -1);

	printf("test_list_remove: passou!");
}

/**************************************************************/
/*
 * Testa a destruição de uma lista. Se este teste for efetuado
 * com sucesso, o programa imprime "passou!".
 */
void test_list_destroy() {
	struct list_t* good_list1 = list_create();
	struct list_t* good_list2 = list_create();
	struct list_t* bad_list;

	struct data_t* data1 = data_create(25, 12);
	struct entry_t* entry1 = entry_create("chave", data1);
	struct data_t* data2 = data_create(100, "bruh");
	struct entry_t* entry2 = entry_create("chavona", data2);

	list_add(good_list2, entry1);
	list_add(good_list2, entry2);

	assert(list_destroy(good_list1) == 0);
	assert(list_destroy(bad_list) == -1);
	assert(list_destroy(good_list2) == 0);

	printf("test_list_destroy: passou!");
}

/**************************************************************/
/*
 * Testa a função que obtém uma entry de uma lista. Se este
 * teste for efetuado com sucesso, o programa imprime "passou!".
 */
void test_list_get() {
	struct list_t* good_list = list_create();
	struct list_t* bad_list;

	struct data_t* data1 = data_create(25, 12);
	struct entry_t* entry1 = entry_create("chave", data1);
	struct data_t* data2 = data_create(100, "bruh");
	struct entry_t* entry2 = entry_create("chavona", data2);
	struct data_t* data3 = data_create(5, 0);
	struct entry_t* entry3 = entry_create("chave", data3);  // entry com chave igual à entry1

	list_add(good_list, entry1);
	list_add(good_list, entry2);

	assert(list_get(bad_list, "chave") == NULL);
	assert(list_get(good_list, "ronaldo") == NULL);
	assert(entry_compare(list_get(good_list, "chave"), entry1) == 0);
	assert(entry_compare(list_get(good_list, "chave"), entry3) == 0);

	list_add(good_list, entry3);	// substitui a entry com a key "chave"
	assert(entry_compare(list_get(good_list, "chave"), entry1) == 0);
	assert(entry_compare(list_get(good_list, "chave"), entry3) == 0);

	printf("test_list_get: passou!");
}

/**************************************************************/
/*
 * Testa a função que indica o tamanho de uma lista. Se este
 * teste for efetuado com sucesso, o programa imprime "passou!".
 */
void test_list_size() {
	struct list_t* good_list = list_create();
	struct list_t* bad_list;

	assert(list_size(bad_list) == -1);
	assert(list_size(good_list) == 0);

	struct data_t* data1 = data_create(25, 12);
	struct entry_t* entry1 = entry_create("chave", data1);
	struct data_t* data2 = data_create(100, "bruh");
	struct entry_t* entry2 = entry_create("chavona", data2);
	struct data_t* data3 = data_create(5, 0);
	struct entry_t* entry3 = entry_create("chave", data3);  // entry com chave igual à entry1

	list_add(good_list, entry1);
	list_add(good_list, entry2);
	list_add(good_list, entry3);	// substitui entry1

	assert(list_size(good_list) == 2);

	list_remove(good_list, "chave");
	assert(list_size(good_list) == 1);

	printf("test_list_size: passou!");
}

/**************************************************************/
/*
 * Testa a função que devolve uma cópia de todas as keys na
 * lista. Se este teste for efetuado com sucesso, o programa
 * imprime "passou!".
 */
void test_list_get_keys() {
	struct list_t* good_list = list_create();
	struct list_t* bad_list;

	assert(list_get_keys(bad_list) == NULL);
	char** result1 = list_get_keys(good_list);
	assert((sizeof(result1) / sizeof(result1[0])) == 0);	// 0 keys

	struct data_t* data1 = data_create(25, 12);
	struct entry_t* entry1 = entry_create("chave", data1);
	struct data_t* data2 = data_create(100, "bruh");
	struct entry_t* entry2 = entry_create("chavona", data2);

	list_add(good_list, entry1);
	list_add(good_list, entry2);

	char** result2 = list_get_keys(good_list);
	assert((sizeof(result2) / sizeof(result2[0])) == 2);	// 2 keys
}

/**************************************************************/
/*
 * Testa a função que liberta a memória ocupada pelo array de
 * keys obtido pela função list_get_keys. Se este teste for
 * efetuado com sucesso, o programa imprime "passou!".
 */
void test_list_free_keys() {
	struct list_t* good_list = list_create();
	struct list_t* bad_list;

	// TODO
}

/**************************************************************/
int main() {
	return 0;
}
