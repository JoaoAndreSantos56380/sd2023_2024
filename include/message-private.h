// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "sdmessage.pb-c.h"
#include "client_stub.h"
#include "client_stub-private.h"

#define message_t _MessageT

/* Esta função lê os bytes do buffer um por um
*  do servidor, devolvendo os bytes que foram lidos.
*  Em caso de erro devolve -1.
*/
int read_all(int socket, char** buf, int size);

/* Esta função escreve os bytes do buffer um por um
*  ao servidor, devolvendo os bytes que foram escritos.
*  Em caso de erro devolve -1.
*/
int write_all(int socket, char* buf, int size);
#endif
