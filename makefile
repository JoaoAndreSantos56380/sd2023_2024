# Grupo 21
# Joao Santos 56380
# Rafael Ferreira 57544
# Ricardo Mateus 56366

# Diretório de objetos
OBJ_DIR = object
INC_DIR = include
LIB_DIR = lib
BIN_DIR = binary
SRC_DIR = source
DEP_DIR = dependencies

CC = gcc

CFLAGS = -Wall -g -I $(INC_DIR)

EXECS = table-server table-client

# Source files and object files
make: $(EXECS)

all: libtable table-server table-client

run:
	$(BIN_DIR)/test_data
	$(BIN_DIR)/test_entry
	$(BIN_DIR)/test_list
	$(BIN_DIR)/test_table
	$(BIN_DIR)/test_serialization

$(BIN_DIR)/test_data: $(OBJ_DIR)/test_data.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_entry: $(OBJ_DIR)/test_entry.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_list: $(OBJ_DIR)/test_list.o $(OBJ_DIR)/list.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_serialization: $(OBJ_DIR)/test_serialization.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o $(OBJ_DIR)/list.o $(OBJ_DIR)/serialization.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_table: $(OBJ_DIR)/test_table.o $(OBJ_DIR)/table.o $(OBJ_DIR)/list.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/table-client: $(OBJ_DIR)/table_client.o $(LIB_DIR)/libtable.a $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/sdmessage.pb-c.o
	$(CC) $^ -o $(BIN_DIR)/table-client

$(LIB_DIR)/libtable.a: $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o $(OBJ_DIR)/table.o
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm -rf $(DEP_DIR)/* $(OBJ_DIR)/* $(BIN_DIR)/* $(LIB_DIR)/*

build:
	mkdir $(LIB_DIR)
	mkdir $(OBJ_DIR)
	mkdir $(BIN_DIR)

$(OBJ_DIR)/sdmessage.pb-c.o:
	protoc --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c source
	mv sdmessage.pb-c.h include
	$(CC) $(CFLAGS) -c $(SRC_DIR)/sdmessage.pb-c.c -o $(OBJ_DIR)/sdmessage.pb-c.o

client_stub.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client_stub.c -o $(OBJ_DIR)/client_stub.o

network_client.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/network_client.c -o $(OBJ_DIR)/network_client.o

message-private.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/message-private.c -o $(OBJ_DIR)/message-private.o

network_server.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/network_server.c -o $(OBJ_DIR)/network_server.o

table_client.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/table_client.c -o $(OBJ_DIR)/table_client.o

table_skel.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/table_skel.c -o $(OBJ_DIR)/table_skel.o

table-server: libtable $(OBJ_DIR)/sdmessage.pb-c.o network_server.o table_skel.o message-private.o
	$(CC) $(CFLAGS) $(SRC_DIR)/table_server.c -o $(BIN_DIR)/table-server $(OBJ_DIR)/network_server.o $(OBJ_DIR)/table_skel.o $(LIB_DIR)/libtable.a $(OBJ_DIR)/message-private.o $(OBJ_DIR)/sdmessage.pb-c.o -I/usr/include/ -L/usr/include -lprotobuf-c -lpthread -lm

server_run: table-server
	./$(BIN_DIR)/table-server 1337 4

libtable: data.o entry.o list.o table.o
	ar -rcs $(LIB_DIR)/libtable.a $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o $(OBJ_DIR)/table.o

table-client: libtable client_stub.o $(OBJ_DIR)/sdmessage.pb-c.o message-private.o network_client.o
	$(CC) $(CFLAGS) $(SRC_DIR)/table_client.c -o $(BIN_DIR)/table-client $(LIB_DIR)/libtable.a $(OBJ_DIR)/message-private.o $(OBJ_DIR)/network_client.o $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/sdmessage.pb-c.o -I/usr/include/ -L/usr/include -lprotobuf-c

client_run: table-client
	./$(BIN_DIR)/table-client 127.0.0.1:1337 < teste1.txt

cclient_valgrind2: table-client
	valgrind --leak-check=full --track-origins=yes $(BIN_DIR)/table-client 127.0.0.1:1337 < ./teste1.txt
cclient_valgrind1: table-client
	valgrind --leak-check=full --track-origins=yes $(BIN_DIR)/table-client 127.0.0.1:1337 < ./teste2.txt

cclient_valgrind3: table-client
	valgrind --leak-check=full --track-origins=yes $(BIN_DIR)/table-client 127.0.0.1:1337

sserver_valgrind: table-server
	valgrind --leak-check=full --track-origins=yes $(BIN_DIR)/table-server 1337 4

data.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/data.c -o $(OBJ_DIR)/data.o
entry.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/entry.c -o $(OBJ_DIR)/entry.o
list.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o
table.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/table.c -o $(OBJ_DIR)/table.o
