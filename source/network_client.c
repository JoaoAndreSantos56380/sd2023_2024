#include "network_client.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client_stub-private.h"
#include "message-private.h"
#include "client_stub.h"
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
	// alteramos os returns dos erros para NULL?
	int sockfd = rtable->sockfd;
	// variavel temporaria para guardar os bytes
	int nbytes;


	int size_msg = message_t__get_packed_size(msg);
	char* buffer = (char*)malloc(size_msg);
	int buffer_size = message_t__pack(msg, (uint8_t*)buffer);
	MessageT* str_de_serialized;

	if ((nbytes = write_all(sockfd, buffer, buffer_size)) != buffer_size) {
		perror("Erro ao enviar dados ao servidor");
		close(sockfd);
		return NULL;
	}

	buffer = (char*)realloc(buffer, 1024);
	nbytes = read_all(sockfd, &buffer, 1024);

	str_de_serialized = message_t__unpack(NULL, nbytes, (uint8_t*)buffer);

	return str_de_serialized;
}


int network_close(struct rtable_t* rtable) {
	return close(rtable->sockfd);
}
