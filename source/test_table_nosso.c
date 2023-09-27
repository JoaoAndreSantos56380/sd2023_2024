#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "table.h"

/**************************************************************/
int testAll1() {
	int result, i;
	struct table_t* table = table_create(20);
	char* key[2048];
	struct data_t *data[2048], *d;

	printf("Módulo table -> testAll1: ");
	fflush(stdout);

	for (i = 0; i < 2048; i++) {
		key[i] = (char*)malloc(16 * sizeof(char));
		sprintf(key[i], "a/key/b-%d", i);
		data[i] = data_create(strlen(key[i]) + 1, strdup(key[i]));

		table_put(table, key[i], data[i]);
	}

	assert(table_size(table) == 2048);
	result = (table_size(table) == 2048);

	d = data_create(strlen("348") + 1, strdup("348"));
	table_put(table, key[348], d);
	data_destroy(d);

	assert(table_size(table) == 2048);
	result = result && (table_size(table) == 2048);

	for (i = 0; i < 2048; i++) {
		d = table_get(table, key[i]);

		if (i == 348) {
			result = result && (d->datasize == strlen("348") + 1 &&
									  memcmp(d->data, "348", d->datasize) == 0);
		} else {
			result = result && (d->datasize == data[i]->datasize &&
									  memcmp(d->data, data[i]->data, d->datasize) == 0 &&
									  d->data != data[i]->data);
		}

		data_destroy(d);
	}

	for (i = 0; i < 2048; i++) {
		free(key[i]);
		data_destroy(data[i]);
	}

	table_destroy(table);

	printf("%s\n", result ? "passou" : "não passou");
	return result;
}

/**************************************************************/
int testAll2() {
	int result, i;
	struct table_t* table = table_create(20);
	char* key[16384];
	struct data_t *data[16384], *d, *data2, *data3, *data4, *deita, *deita2, *deita3;

	printf("Módulo table -> testAll2: ");
	fflush(stdout);

	for (i = 0; i < 16384; i++) {
		key[i] = (char*)malloc(16 * sizeof(char));
		sprintf(key[i], "a/key/b-%d", i);
		data[i] = data_create(strlen(key[i]) + 1, strdup(key[i]));

		table_put(table, key[i], data[i]);
		//data_destroy(data[i]);
	}

	assert(table_size(table) == 16384);
	result = (table_size(table) == 16384);

	result = result && ((data2 = table_get(table, "a/key/b-4000")) != NULL) &&
				((data3 = table_get(table, "a/key/b-6000")) != NULL) && ((data4 = table_get(table, "a/key/b-11000")) != NULL);

	d = data_create(strlen("4000") + 1, strdup("4000"));
	table_put(table, key[4000], d);

	assert(memcmp(d->data, "4000", d->datasize) == 0);
	result = result && (memcmp(d->data, "4000", d->datasize) == 0);

	data_destroy(d);

	assert(table_size(table) == 16384);
	result = result && (table_size(table) == 16384);

	table_remove(table, key[4000]);
	table_remove(table, key[11000]);

	assert(table_size(table) == 16382);
	result = result && (table_size(table) == 16382);

	assert(table_get(table, key[4000]) == NULL);
	result = result && ((deita = table_get(table, key[4000])) == NULL);

	d = data_create(strlen("6500") + 1, strdup("6500"));
	table_put(table, key[6500], d);
	data_destroy(d);

	d = data_create(strlen("2000") + 1, strdup("2000"));
	table_put(table, key[2000], d);
	data_destroy(d);

	d = data_create(strlen("11000") + 1, strdup("11000"));
	table_put(table, key[11000], d);
	data_destroy(d);

	assert((deita2 = table_get(table, key[11000])) != NULL);
	result = result && ((deita3 = table_get(table, key[11000])) != NULL);

	assert(table_size(table) == 16383);
	result = result && (table_size(table) == 16383);

	for (i = 0; i < 16384; i++) {
		d = table_get(table, key[i]);

		if (i == 2000) {
			result = result && (d->datasize == strlen("2000") + 1 &&
									  memcmp(d->data, "2000", d->datasize) == 0);
		} else {
			if (i == 4000) {
				result = result && (table_get(table, key[4000]) == NULL);
			} else {
				if (i == 6500) {
					result = result && (d->datasize == strlen("6500") + 1 &&
											  memcmp(d->data, "6500", d->datasize) == 0);
				} else {
					if (i == 11000) {
						result = result && (d->datasize == strlen("11000") + 1 &&
												  memcmp(d->data, "11000", d->datasize) == 0);
					} else {
						result = result && (d->datasize == data[i]->datasize &&
												  memcmp(d->data, data[i]->data, d->datasize) == 0 &&
												  d->data != data[i]->data);
					}
				}
			}
		}

		data_destroy(d);
	}

	for (i = 0; i < 16384; i++) {
		free(key[i]);
		data_destroy(data[i]);
	}
	table_destroy(table);
	data_destroy(data2);
	data_destroy(data3);
	data_destroy(data4);
	data_destroy(deita);
	data_destroy(deita2);
	data_destroy(deita3);

	printf("%s\n", result ? "passou" : "não passou");
	return result;
}

/**************************************************************/
int main() {
	int score = 0;

	printf("\nIniciando teste do módulo table \n");

	score += testAll1();
	score += testAll2();

	printf("teste table (score): %d/2\n", score);

	if (score == 2)
		return 0;
	else
		return -1;
}
