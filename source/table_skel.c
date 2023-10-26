#include "sdmessage.pb-c.h"
#include "table-private.h"
#include "table.h"

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna a tabela criada ou NULL em caso de erro.
 */
//struct table_t* table;

struct table_t* table_skel_init(int n_lists) {
	// Inicializar a tabela com n_lists
	struct table_t* table = table_create(n_lists);

	return table;
}

/* Liberta toda a memória ocupada pela tabela e todos os recursos
 * e outros recursos usados pelo skeleton.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_skel_destroy(struct table_t* table) {
	if (table == NULL) {
		// A tabela não foi inicializada, não há nada para destruir!
		return -1;
	}

	// Destruir a tabela
	table_destroy(table);

	return 0;
}

/* Executa na tabela table a operação indicada pelo opcode contido em msg
 * e utiliza a mesma estrutura MessageT para devolver o resultado.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
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
			// int result = table_put(table, msg->entry->key, (struct data_t*) msg->entry->value.data);
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

			return 0;
			break;
		case MESSAGE_T__OPCODE__OP_GET:
			// Verificar se o campo da mensagem é válido
			if (msg->key == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Fazer a operação na tabela
			struct data_t* result_data = table_get(table, msg->key);
			//printf("%s", result_data->data);
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
			break;
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
			if (del_result == -1) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

			return 0;
			break;
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
			break;

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
			break;

		case MESSAGE_T__OPCODE__OP_GETTABLE:
			// Fazer a operação na tabela
			// struct table_t *result_table = table_get_table(table);

			// Erro ao buscar a table
			// if(result_table == NULL) {
			if (table == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Buscar as entries
			int num_entries = 0;
			// struct entry_t **all_entries = get_all_entries(result_table, &num_entries);
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
			msg->entries = (EntryT**)all_entries; // com este não dá erro de compilação
			//msg->n_entries = num_entries;

			return 0;
			break;

		default:
			// Opcode inválido
			msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

			return -1;
			break;
	}
}
