#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client_stub-private.h"
#include "client_stub.h"
#include "message-private.h"
#include "network_client.h"
#include "sdmessage.pb-c.h"


struct rtable_t* rtable_connect(char* address_port) {
	char* adrsport = (char*)malloc(strlen(address_port) + 1);
	strcpy(adrsport, address_port);
	char* hostname = strdup(strtok(adrsport, ":"));
	char* port = strdup(strtok(NULL, ":"));
	struct rtable_t* rtable = (struct rtable_t*)malloc(sizeof(struct rtable_t));
	rtable->server_address = hostname;
	rtable->server_port = atoi(port);
	rtable->sockfd = -1;
	free(adrsport);
	return network_connect(rtable) == -1 ? NULL : rtable;
}


int rtable_disconnect(struct rtable_t* rtable) {
	if (network_close(rtable) == -1) {
		printf("Error closing connection.\n");
	}
	free(rtable->server_address);
	free(rtable);
	printf("Closing\n");
	return 0;
}


int rtable_put(struct rtable_t* rtable, struct entry_t* entry) {
	MessageT* request = (MessageT*)malloc(sizeof(MessageT));
	message_t__init(request);
	request->opcode = MESSAGE_T__OPCODE__OP_PUT;
	request->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
	request->entry = (EntryT*)malloc(sizeof(EntryT));
	entry_t__init(request->entry);
	request->entry->key = entry->key;
	request->entry->value.len = entry->value->datasize;
	request->entry->value.data = entry->value->data;
	MessageT* response = network_send_receive(rtable, request);
	free(request->entry);
	free(request);
	int result = (response->opcode == MESSAGE_T__OPCODE__OP_PUT + 1) ? 0 : -1;
	message_t__free_unpacked(response, NULL);
	return result;
}


struct data_t* rtable_get(struct rtable_t* rtable, char* key) {
	struct message_t* request = (struct message_t*)malloc(sizeof(struct message_t));
	message_t__init(request);
	request->opcode = MESSAGE_T__OPCODE__OP_GET;
	request->c_type = MESSAGE_T__C_TYPE__CT_KEY;
	request->key = key;
	struct message_t* response = network_send_receive(rtable, request);
	free(request);
	if (response->opcode != (MESSAGE_T__OPCODE__OP_GET + 1)) {
		return NULL;
	}
	struct data_t* data = (struct data_t*)malloc(sizeof(struct data_t));
	data->datasize = response->value.len;
	data->data = malloc(response->value.len);
	memcpy(data->data, response->value.data, response->value.len);
	message_t__free_unpacked(response, NULL);
	return data;
}


int rtable_del(struct rtable_t* rtable, char* key) {
	struct message_t* request = (struct message_t*)malloc(sizeof(struct message_t));
	message_t__init(request);
	request->opcode = MESSAGE_T__OPCODE__OP_DEL;
	request->c_type = MESSAGE_T__C_TYPE__CT_KEY;
	request->key = key;
	struct message_t* response = network_send_receive(rtable, request);
	free(request);
	int result = response->opcode != MESSAGE_T__OPCODE__OP_ERROR ? response->result : -1;
	message_t__free_unpacked(response, NULL);
	return result;
}


int rtable_size(struct rtable_t* rtable) {
	struct message_t* request = (struct message_t*)malloc(sizeof(struct message_t));
	message_t__init(request);
	request->opcode = MESSAGE_T__OPCODE__OP_SIZE;
	request->c_type = MESSAGE_T__C_TYPE__CT_NONE;
	struct message_t* response = network_send_receive(rtable, request);
	free(request);
	int result = response->opcode != MESSAGE_T__OPCODE__OP_ERROR ? response->result : -1;
	message_t__free_unpacked(response, NULL);
	return result;
}


char** rtable_get_keys(struct rtable_t* rtable) {
	struct message_t* request = (struct message_t*)malloc(sizeof(struct message_t));
	message_t__init(request);
	request->opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
	request->c_type = MESSAGE_T__C_TYPE__CT_NONE;
	struct message_t* response = network_send_receive(rtable, request);
	free(request);
	if (response->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
		message_t__free_unpacked(response, NULL);
		return NULL;
	}

	char** keys = (char**)malloc(sizeof(char*) * (response->n_keys + 1));
	int i;
	for (i = 0; i < response->n_keys; i++) {
		keys[i] = strdup(response->keys[i]);
	}
	keys[i] = NULL;
	message_t__free_unpacked(response, NULL);
	return keys;
}


struct entry_t** rtable_get_table(struct rtable_t* rtable){
	struct message_t* request = (struct message_t*)malloc(sizeof(struct message_t));
	message_t__init(request);
	request -> opcode = MESSAGE_T__OPCODE__OP_GETTABLE;
	request -> c_type = MESSAGE_T__C_TYPE__CT_NONE;
	struct message_t* response = network_send_receive(rtable, request);
	free(request);
	if (response->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
		message_t__free_unpacked(response, NULL);
		return NULL;
	}

	struct entry_t** entries = (struct entry_t**)malloc(sizeof(struct entry_t*) * (response->n_entries + 1));
	int i;
	struct entry_t* entry = malloc(sizeof(struct entry_t));
	entry->key = malloc(sizeof(char*));
	for (i = 0; i < response->n_entries; i++) {
		strcpy(entry->key, response->entries[i]->key);
		entry->value = data_create(response->entries[i]->value.len, response->entries[i]->value.data);
		entries[i] = entry_dup(entry);
	}
	entries[i] = NULL;
	return entries;
}


void rtable_free_entries(struct entry_t** entries){
	if(entries != NULL){
		int i = 0;

		while(entries[i] != NULL){
			entry_destroy(entries[i]);
		}
		free(entries);
	}
}
