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
int read_all(int socket, void *buf, int size);

/* Esta função escreve os bytes do buffer um por um
*  ao servidor, devolvendo os bytes que foram escritos.
*  Em caso de erro devolve -1.
*/
int write_all(int socket, void *buf, int size);

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 * - Em caso de erro, devolve null.
 */
struct messageT *message_receive(int client_socket);

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 * - Em caso de erro, devolve -1.
 */
int message_send(int client_socket, struct messageT *msg);

#endif
