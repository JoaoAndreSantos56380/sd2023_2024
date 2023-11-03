// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdmessage.pb-c.h"
#include "table-private.h"
#include "table.h"
#include "stats.h"

struct table_t* table_skel_init(int n_lists) {
	// Inicializar a tabela com n_lists
	struct table_t* table = table_create(n_lists);

	return table;
}

int table_skel_destroy(struct table_t* table) {
	if (table == NULL) {
		// A tabela não foi inicializada, não há nada para destruir!
		return -1;
	}

	// Destruir a tabela
	table_destroy(table);

	return 0;
}

int invoke(MessageT* msg, struct table_t* table) {
	if (msg == NULL || table == NULL) {
		// A msg ou a table são inválidas!
		msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
		msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
		return -1;
	}

	// Verificar qual o opcode e realizar a operação de acordo com esse opcode.
	switch (msg->opcode) {
		case MESSAGE_T__OPCODE__OP_PUT:
			// Verificar se os campos da mensagem são válidos
			if (msg->entry == NULL || msg->entry->key == NULL || msg->entry->value.data == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}
			// Fazer a operação na tabela
			struct data_t* data = data_create(msg->entry->value.len, msg->entry->value.data);

			int result = table_put(table, msg->entry->key, data);

			// Erro ao colocar na table
			if (result == -1) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_PUT + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
			data_destroy(data);
			return 0;

		case MESSAGE_T__OPCODE__OP_GET:
			// Verificar se o campo da mensagem é válido
			if (msg->key == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Fazer a operação na tabela
			struct data_t* result_data = table_get(table, msg->key);
			// Erro ao buscar da table
			if (result_data == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_GET + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
			msg->value.data = result_data->data;
			msg->value.len = result_data->datasize;
			return 0;

		case MESSAGE_T__OPCODE__OP_DEL:
			// Verificar se o campo da mensagem é válido
			if (msg->key == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Fazer a operação na tabela
			int del_result = table_remove(table, msg->key);

			// Erro ao apagar da table
			if (del_result == -1 || del_result == 1) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
			return 0;

		case MESSAGE_T__OPCODE__OP_SIZE:
			// Fazer a operação na tabela
			int size = table_size(table);

			// Erro ao buscar o size da table
			if (size == -1) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_SIZE + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
			msg->result = size;
			return 0;

		case MESSAGE_T__OPCODE__OP_GETKEYS:
			// Fazer a operação na tabela
			char** keys = table_get_keys(table);
			size_t n_keys = table_size(table);

			// Erro ao buscar os parametros
			if (keys == NULL || n_keys == -1) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_GETKEYS + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
			msg->keys = keys;
			msg->n_keys = n_keys;
			return 0;

		case MESSAGE_T__OPCODE__OP_GETTABLE:
			if (table == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Buscar as entries
			int num_entries = 0;
			struct entry_t** all_entries = get_all_entries(table, &num_entries);

			// Erro ao buscar as entries
			if (all_entries == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_GETTABLE + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_TABLE;
			msg->entries = malloc(sizeof(EntryT*) * (num_entries + 1));
			for (int i = 0; i < num_entries; i++) {
				struct entry_t* entry = all_entries[i];
				msg->entries[i] = (EntryT*)malloc(sizeof(EntryT));
				entry_t__init(msg->entries[i]);
				msg->entries[i]->key = malloc(strlen(entry->key) + 1);
				strcpy(msg->entries[i]->key, all_entries[i]->key);
				msg->entries[i]->value.len = entry->value->datasize;
				msg->entries[i]->value.data = malloc(entry->value->datasize);
				memcpy(msg->entries[i]->value.data, entry->value->data, entry->value->datasize);
			}

			msg->entries[num_entries] = NULL;
			msg->n_entries = num_entries;
			for (int i = 0; i < num_entries; i++) {
				entry_destroy(all_entries[i]);
			}
			free(all_entries);
			return 0;

		case MESSAGE_T__OPCODE__OP_STATS:
			// É a única operação que não precisamos de chamar funções da table
			 struct statistics_t server_stats;

			
			//TODO

		default:
			// Opcode inválido
			msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
			return -1;
	}
}
