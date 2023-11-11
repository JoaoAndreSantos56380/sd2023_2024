// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "sdmessage.pb-c.h"
#include "stats.h"
#include "table-private.h"
#include "table.h"
#include "table_skel-private.h"
#include "table_skel.h"

pthread_mutex_t table_lock;
pthread_mutex_t stats_lock;

// Stats
struct statistics_t server_stats;  // ATENÇÃO: Verificar se dá erro, talvez tenhamos de inicializar no init do table_skel

struct table_t* table_skel_init(int n_lists) {
	// Inicializar a tabela com n_lists
	struct table_t* table = table_create(n_lists);

	// server_stats = malloc(sizeof(statistics_t));
	server_stats.num_clients_connected = 0;
	server_stats.num_ops = 0;
	server_stats.total_time_microseconds = 0;

	// Inicializar o table_lock
	if (pthread_mutex_init(&table_lock, NULL) != 0) {
		printf("Table mutex init has failed\n");
		return NULL;
	}
	// Inicializar o stats_lock
	if (pthread_mutex_init(&stats_lock, NULL) != 0) {
		printf("Stats mutex init has failed\n");
		return NULL;
	}

	return table;
}

int table_skel_destroy(struct table_t* table) {
	if (table == NULL) {
		// A tabela não foi inicializada, não há nada para destruir!
		return -1;
	}

	// Destruir a tabela
	table_destroy(table);

	// Destroy mutexes
	pthread_mutex_destroy(&table_lock);
	pthread_mutex_destroy(&stats_lock);

	return 0;
}

int invoke(MessageT* msg, struct table_t* table) {
	if (msg == NULL || table == NULL) {
		// A msg ou a table são inválidas!
		msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
		msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
		return -1;
	}

	struct timeval start_time, end_time;
	// Verificar qual o opcode e realizar a operação de acordo com esse opcode.
	switch (msg->opcode) {
		case MESSAGE_T__OPCODE__OP_PUT:
			// Inicializar o start_time para os stats
			gettimeofday(&start_time, NULL);

			// Verificar se os campos da mensagem são válidos
			if (msg->entry == NULL || msg->entry->key == NULL || msg->entry->value.data == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}
			// Fazer a operação na tabela
			struct data_t* data = data_create(msg->entry->value.len, msg->entry->value.data);
			pthread_mutex_lock(&table_lock);
			int result = table_put(table, msg->entry->key, data);
			pthread_mutex_unlock(&table_lock);

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

			// Atualizar os stats (tempo e num_ops)
			gettimeofday(&end_time, NULL);
			update_stats(start_time, end_time);

			return 0;

		case MESSAGE_T__OPCODE__OP_GET:
			// Inicializar o start_time para os stats
			gettimeofday(&start_time, NULL);

			// Verificar se o campo da mensagem é válido
			if (msg->key == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Fazer a operação na tabela
			pthread_mutex_lock(&table_lock);
			struct data_t* result_data = table_get(table, msg->key);
			pthread_mutex_unlock(&table_lock);
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

			// Atualizar os stats (tempo e num_ops)
			gettimeofday(&end_time, NULL);
			update_stats(start_time, end_time);

			return 0;

		case MESSAGE_T__OPCODE__OP_DEL:
			// Inicializar o start_time para os stats
			gettimeofday(&start_time, NULL);

			// Verificar se o campo da mensagem é válido
			if (msg->key == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Fazer a operação na tabela
			pthread_mutex_lock(&table_lock);
			int del_result = table_remove(table, msg->key);
			pthread_mutex_unlock(&table_lock);

			// Erro ao apagar da table
			if (del_result == -1 || del_result == 1) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;

			// Atualizar os stats (tempo e num_ops)
			gettimeofday(&end_time, NULL);
			update_stats(start_time, end_time);

			return 0;

		case MESSAGE_T__OPCODE__OP_SIZE:
			// Inicializar o start_time para os stats
			gettimeofday(&start_time, NULL);

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

			// Atualizar os stats (tempo e num_ops)
			gettimeofday(&end_time, NULL);
			update_stats(start_time, end_time);

			return 0;

		case MESSAGE_T__OPCODE__OP_GETKEYS:
			// Inicializar o start_time para os stats
			gettimeofday(&start_time, NULL);

			// Fazer a operação na tabela
			pthread_mutex_lock(&table_lock);
			char** keys = table_get_keys(table);
			pthread_mutex_unlock(&table_lock);
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

			// Atualizar os stats (tempo e num_ops)
			gettimeofday(&end_time, NULL);
			update_stats(start_time, end_time);

			return 0;

		case MESSAGE_T__OPCODE__OP_GETTABLE:
			// Inicializar o start_time para os stats
			gettimeofday(&start_time, NULL);

			if (table == NULL) {
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}

			// Buscar as entries
			int num_entries = 0;
			pthread_mutex_lock(&table_lock);
			struct entry_t** all_entries = get_all_entries(table, &num_entries);
			pthread_mutex_unlock(&table_lock);

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

			// Atualizar os stats (tempo e num_ops)
			gettimeofday(&end_time, NULL);
			update_stats(start_time, end_time);

			return 0;

		case MESSAGE_T__OPCODE__OP_STATS:  // ATENÇÃO | Como vemos erros no stats?
			// Atualizar a estrutura MessageT com o resultado
			msg->opcode = MESSAGE_T__OPCODE__OP_STATS + 1;
			msg->c_type = MESSAGE_T__C_TYPE__CT_STATS;
			msg->stats = (StatisticsT*)malloc(sizeof(StatisticsT));
			statistics_t__init(msg->stats);
			if (msg->stats == NULL) {	// Erro ao alocar memória
				msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
				msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
				return -1;
			}
			msg->stats->ops = (int32_t)server_stats.num_ops;
			msg->stats->total_time = (uint64_t)server_stats.total_time_microseconds;
			msg->stats->clients = (int32_t)server_stats.num_clients_connected;

			return 0;

		default:
			// Opcode inválido
			msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
			msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
			return -1;
	}
}

void update_server_stats_clients(int n, int op) {
	if (op == 0) {
		server_stats.num_clients_connected += n;
	} else {
		server_stats.num_clients_connected -= n;
	}
}

void update_stats(struct timeval start_time, struct timeval end_time) {
	pthread_mutex_lock(&stats_lock);
	server_stats.total_time_microseconds += (end_time.tv_usec - start_time.tv_usec);
	server_stats.num_ops += 1;
	pthread_mutex_unlock(&stats_lock);
}
