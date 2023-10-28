// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#include "message-private.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sdmessage.pb-c.h"

//#define message_t MessageT

int write_all(int socket, char* buf, int size) {
	int bufsize = size;
	while (size > 0) {
		int res = write(socket, buf, size);
		if (res < 0) {
			if (errno == EINTR)
				continue;
			perror("write failed:");
			return res;
		}
		buf += res;
		size -= res;
	}
	return bufsize;
}

int read_all(int sock, char** buf_ptr, int read_n_bytes) {
	int bytes_read = 0;
	int total_bytes_read = 0;
	int fullBuffer;
	int buffer_size = read_n_bytes;
	do {
		bytes_read = read(sock, (*buf_ptr) + total_bytes_read, read_n_bytes);
		if(bytes_read < 0) {
			if (errno == EINTR)
				continue;
			perror("read failed:");
			return bytes_read;
		}
		fullBuffer = bytes_read == read_n_bytes;
		if (fullBuffer) {
			buffer_size += read_n_bytes;
			*buf_ptr = (char*)realloc(*buf_ptr, buffer_size);
		}
		total_bytes_read += bytes_read;
	} while (fullBuffer);
	return total_bytes_read;
}

