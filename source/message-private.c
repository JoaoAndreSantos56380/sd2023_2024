#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "message-private.h"

#define message_t MessageT

/* Esta função lê os bytes do buffer um por um
*  do servidor, devolvendo os bytes que foram lidos.
*  Em caso de erro devolve -1.
*/
int read_all(int socket, void *buf, int size) {
    int total_bytes_read = 0;

    int bytes_read;
    while(total_bytes_read < size) {
        bytes_read = read(socket, buf + total_bytes_read, size - total_bytes_read);
        if(bytes_read < 0) {
            perror("Error reading from socket");
            return -1;
        } else if(bytes_read == 0) {
            // Connection closed
            return -1;
        }
        total_bytes_read += bytes_read;
    }

    return total_bytes_read;
}

/* Esta função escreve os bytes do buffer um por um
*  ao servidor, devolvendo os bytes que foram escritos.
*  Em caso de erro devolve -1.
*/
int write_all(int socket, void *buf, int size) {
    int total_bytes_written = 0;

    int bytes_written;
    while(total_bytes_written < size) {
        bytes_written = write(socket, buf + total_bytes_written, size - total_bytes_written);
        if(bytes_written < 0) {
            perror("Error writing to socket");
            return -1;
        }
        total_bytes_written += bytes_written;
    }

    return total_bytes_written;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 * - Em caso de erro, devolve null.
 */
struct messageT *message_receive(int client_socket) {
    uint32_t message_length;
    if(read_all(client_socket, &message_length, sizeof(uint32_t)) < 0) {
        return NULL;
    }

    char *buffer = (char *)malloc(message_length);
    if(buffer == NULL) {
        perror("Error allocating memory for message");
        return NULL;
    }

    if(read_all(client_socket, buffer, message_length) < 0) {
        free(buffer);
        return NULL;
    }

    struct messageT *request = message_t__unpack(NULL, message_length, buffer);
    free(buffer);

    if(request == NULL) {
        fprintf(stderr, "Error unpacking message\n");
    }

    return request;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int message_send(int client_socket, struct messageT *msg) {
    uint32_t message_length = message_t__get_packed_size(msg);
    char *buffer = (char *)malloc(message_length);
    if(buffer == NULL) {
        perror("Error allocating memory for message");
        return -1;
    }

    message_t__pack(msg, (uint8_t *)buffer);

    if(write_all(client_socket, &message_length, sizeof(uint32_t) < 0)) {
        free(buffer);
        return -1;
    }

    if(write_all(client_socket, buffer, message_length) < 0) {
        free(buffer);
        return -1;
    }

    free(buffer);
    message_t__free_unpacked(msg, NULL);

    return 0;
}

#endif