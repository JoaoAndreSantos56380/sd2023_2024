#ifndef _NETWORK_CLIENT_H
#define _NETWORK_CLIENT_H

#include "network_client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "client_stub-private.h"
#include "client_stub.h"
#include "sdmessage.pb-c.h"

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) com base na
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t* rtable) {
	struct sockaddr_in serv_addr;
	int ip = atoi(rtable->server_address);
	int port = rtable->server_port;

	// criar o socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Erro na criação do socket");
		return -1;
	}

	// guardar o socket no rtable
	rtable->sockfd = sockfd;

	// inicializar a memoria para n haver erros
	memset(&serv_addr, '\0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	// caso nao funcione nao usar htons()
	serv_addr.sin_port = ntons(port);

	// converter ip, verificar o argv[]
	if (inet_pton(AF_INET, rtable->server_address, &serv_addr.sin_addr) < 1) {
		printf("Erro ao converter IP\n");
		close(sockfd);
		return -1;
	}

	if ((connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		perror("Erro ao conectar-se ao servidor");
		close(sockfd);
		return -1;
	}

	return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Tratar de forma apropriada erros de comunicação;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
MessageT* network_send_receive(struct rtable_t* rtable, MessageT* msg) {
	// alteramos os returns dos erros para NULL?
	int sockfd = rtable->sockfd;
	// variavel temporaria para guardar os bytes
	int count, nbytes;
	// string que sera serializada com tamanho max de 1024bytes
	char* str_serialized[1024];
	MessageT* str_de_serialized;
	// alocador para o unpacking
	// ProtobufCAllocator  *allocator;

	int bytes = message_t__pack(&msg, str_serialized);

	// Envia string
	if ((nbytes = write(sockfd, str_serialized, bytes)) != bytes) {
		perror("Erro ao enviar dados ao servidor");
		close(sockfd);
		return NULL;
	}

	// Recebe tamanho da string
	if ((nbytes = read(sockfd, &count, sizeof(count))) != sizeof(count)) {
		perror("Erro ao receber dados do servidor");
		close(sockfd);
		return NULL;
	};

	str_de_serialized = message_t__unpack(NULL, nbytes, count);

	return str_de_serialized;
}

/* Fecha a ligação estabelecida por network_connect().
 * Retorna 0 (OK) ou -1 (erro).
 */
int network_close(struct rtable_t* rtable) {
	return close(rtable->sockfd);
}

#endif
