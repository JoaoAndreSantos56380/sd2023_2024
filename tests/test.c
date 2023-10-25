#include <stdio.h>
#include "network_server.h"
#include "client_stub.h"

void test_get_keys_client(struct rtable_t* rtable) {
    char** keys = rtable_get_keys(rtable);
	if (keys == NULL) {
		printf("There was an error executing get_keys() on the server.\n");
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

void teste_put_key_client() {
	strtok(option, " ");
	char* key = strdup(strtok(NULL, " "));
	char* value = strdup(strtok(NULL, " "));
	struct entry_t* entry = entry_create(key, data_create(strlen(value), value));
	if (rtable_put(rtable, entry) == -1) {
		printf("\nput failed\n");
		return;
	}
	entry_destroy(entry);
	printf("\n#######put successful#######\n");
}

int main() {
	struct rtable_t* rtable = rtable_connect("12345");
	teste_put_key_client();
    test_get_keys_client(rtable);
    
    return 0;
}