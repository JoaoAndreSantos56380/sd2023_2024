// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client_stub-private.h"
#include "client_stub.h"
#include "message-private.h"
#include "network_client.h"
#include "sdmessage.pb-c.h"

int network_connect(struct rtable_t* rtable) {
	struct sockaddr_in serv_addr;
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

	serv_addr.sin_port = htons(port);

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

MessageT* network_send_receive(struct rtable_t* rtable, MessageT* msg) {
	int sockfd = rtable->sockfd;
	int nbytes;
	int size_msg = message_t__get_packed_size(msg);
	char* buffer = (char*)malloc(size_msg);
	message_t__pack(msg, (uint8_t*)buffer);
	// Send msg size
	short value = (short)size_msg;
	short htons_value = htons(value);
	int send_result = send(sockfd, &htons_value, sizeof(short), 0);
	if (send_result == -1) {
		perror("send error");
		free(buffer);
		close(sockfd);
		return NULL;
	}

	// Assume write_all is correct
	if ((nbytes = write_all(sockfd, buffer, value)) != value) {
		perror("Erro ao enviar dados ao servidor");
		free(buffer);
		close(sockfd);
		return NULL;
	}

	// Receive response size
	short num;
	int recv_result = recv(sockfd, &num, sizeof(short), 0);
	if (recv_result == -1) {
		perror("recv error");
		free(buffer);
		close(sockfd);
		return NULL;
	}
	num = ntohs(num);

	buffer = (char*)realloc(buffer, num);

	// Assume read_all is correct
	nbytes = read_all(sockfd, &buffer, num);
	if (nbytes != num) {
		perror("Erro ao receber dados do servidor");
		free(buffer);
		close(sockfd);
		return NULL;
	}

	MessageT* deserialized = message_t__unpack(NULL, nbytes, (uint8_t*)buffer);
	free(buffer);	// Free the buffer after use
	return deserialized;
}

int network_close(struct rtable_t* rtable) {
	return close(rtable->sockfd);
}
