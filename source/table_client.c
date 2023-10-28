// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client_stub-private.h"
#include "client_stub.h"
#include "data.h"
#include "entry.h"
#include "table_client-private.h"
#include "sdmessage.pb-c.h"

#define PUT "put"
#define GET "get"
#define DEL "del"
#define QUIT "quit"
#define SIZE "size"
#define HEIGHT "height"
#define GET_KEYS "getkeys"
#define GETTABLE "gettable"

int main(int argc, char const* argv[]) {
	if (argc < 2) {
		printf("Usage: ./table_client <server>:<port>\n");
		printf("Example: ./table_client 127.0.0.1:12345\n");
		return -1;
	}

	struct rtable_t* rtable = rtable_connect((char*)argv[1]);
	if (rtable == NULL) {
		perror("could not connect client\n");
		return -1;
	}

	char option[1024];
	do {
		showMenu();
		readOption(option, 1024);
		if (commandIsPut(option)) {
			executePut(rtable, option);
		} else if (commandIsGetKeys(option)) {
			executeGetKeys(rtable);
		} else if (commandIsGetTable(option)) {
			executeGetTable(rtable);
		} else if (commandIsGet(option)) {
			executeGet(rtable, option);
		} else if (commandIsDel(option)) {
			executeDel(rtable, option);
		} else if (commandIsSize(option)) {
			executeSize(rtable);
		} else {
			printf("Please input a valid command.\n");
		}

	} while (strncmp(option, QUIT, strlen(QUIT)) != 0);
	rtable_disconnect(rtable);
	printf("Client exiting. Bye.\n");
	return 0;
}

void showMenu() {
	printf("\n");
	printf("put <key> <value>\n");
	printf("get <key>\n");
	printf("del <key>\n");
	printf("size\n");
	printf("getkeys\n");
	printf("gettable\n");
	printf("quit\n");
	printf("Option: ");
}

void readOption(char* input, int size) {
	fgets(input, size, stdin);	 // char* input <- stdin até um máximo de size bytes
	input[strlen(input) - 1] = '\0';
}

int commandIsPut(char* option) {
	return strncmp(option, PUT, strlen(PUT)) == 0;
}

int commandIsGet(char* option) {
	return strncmp(option, GET, strlen(GET)) == 0;
}

int commandIsDel(char* option) {
	return strncmp(option, DEL, strlen(DEL)) == 0;
}

int commandIsSize(char* option) {
	return strncmp(option, SIZE, strlen(SIZE)) == 0;
}

int commandIsGetTable(char* option) {
	return strncmp(option, GETTABLE, strlen(GETTABLE)) == 0;
}

int commandIsGetKeys(char* option) {
	return strncmp(option, GET_KEYS, strlen(GET_KEYS)) == 0;
}

void executePut(struct rtable_t* rtable, char* option) {
	char* token = strtok(option, " ");
	char* key = NULL;
	char* value = NULL;

	token = strtok(NULL, " ");
	if (token) { // verificar se o put tem os argumentos ao ser chamado
		key = strdup(token);
		token = strtok(NULL, " ");
		if (token) {
        	value = strdup(token);
    	}
	}
	if (key == NULL || value == NULL) {
		printf("The inserted 'put' command is wrong.\n");
		printf("Example: put 1 abc\n");
		return; // inputs errados => sair da função
	}
	struct entry_t* entry = entry_create(key, data_create(strlen(value), value));
	if (rtable_put(rtable, entry) == -1) {
		printf("\nput failed\n");
		return;
	}
	entry_destroy(entry);
	printf("\n#######put successful#######\n");
}

void executeGet(struct rtable_t* rtable, char* option) {
	char* token = strtok(option, " ");
	char* key = NULL;

	token = strtok(NULL, " ");
	if (token) { // verificar se o get tem o argumento ao ser chamado
		key = strdup(token);
	}
	if (key == NULL) {
		printf("The inserted 'get' command is wrong.\n");
		printf("Example: get 1\n");
		return; // inputs errados => sair da função
	}
	struct data_t* value = rtable_get(rtable, key);
	free(key);
	if (value == NULL) {
		printf("\nget failed\n");
		return;
	}
	char* buffer = (char*)calloc(sizeof(char), value->datasize + 1);
	strncpy(buffer, value->data, value->datasize);	// ja coloca "\0" automaticamente
	printf("\n###Number of bytes: %d\n", value->datasize);
	printf("###Bytes: %s\n", buffer);
	data_destroy(value);
	free(buffer);
}

void executeDel(struct rtable_t* rtable, char* option) {
	char* token = strtok(option, " ");
	char* key = NULL;

	token = strtok(NULL, " ");
	if (token) { // verificar se o get tem o argumento ao ser chamado
		key = strdup(token);
	}
	if(key == NULL) {
		printf("The inserted 'del' command is wrong.\n");
		printf("Example: del 1\n");
		return; // inputs errados => sair da função
	}
	int result = rtable_del(rtable, key);
	if (result == -1) {
		printf("\nDel failed\n");
		return;
	}
	free(key);
	printf("\n #######Del successful####### \n");
}

void executeSize(struct rtable_t* rtable) {
	int result = rtable_size(rtable);
	if (result == -1) {
		printf("\nSize failed\n");
		return;
	}
	printf("\n #######Size successful####### \n");
	printf("Size: %d\n", result);
}

void executeGetKeys(struct rtable_t* rtable) {
	char** keys = rtable_get_keys(rtable);
	if (keys == NULL) {
		printf("There was an error executing get_keys() on the server or there are no keys.\n");
		return;
	}

	if (keys[0] == NULL) {
		printf("\nThere are no keys.\n");
		return;
	}

	printf("\nKeys: \n");
	int i = 0;
	while (keys[i] != NULL) {
		printf("%s\n", keys[i]);
		free(keys[i++]);
	}
	free(keys[i]);
	free(keys);
}

void executeGetTable(struct rtable_t* rtable){
	struct entry_t** entries = (struct entry_t**)rtable_get_table(rtable);
	if (entries == NULL) {
		printf("There was an error executing get_entries() on the server or there are no entries.\n");
		return;
	}

	if (entries[0] == NULL) {
		printf("\nThere are no entries.\n");
		return;
	}

	printf("\nentries: \n");
	int i = 0;
	while (entries[i] != NULL) {
		printf("key:%s\n", (char*)entries[i]->key);
		printf("datasize:%d\n", (int)entries[i]->value->datasize);
		printf("data:%s\n", (char*)entries[i]->value->data);
		entry_destroy(entries[i]);
		i++;
		printf("\n");
	}
	free(entries);
}
