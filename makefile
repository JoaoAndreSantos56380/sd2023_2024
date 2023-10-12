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

EXECS = $(BIN_DIR)/table_client #$(BIN_DIR)/table_server #$(BIN_DIR)/test_data $(BIN_DIR)/test_entry $(BIN_DIR)/test_list $(BIN_DIR)/test_table $(BIN_DIR)/test_serialization

# Source files and object files


make: $(EXECS)

all: libtable table_client #table_server

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

$(BIN_DIR)/table_client: $(OBJ_DIR)/table_client.o $(LIB_DIR)/libtable.a $(OBJ_DIR)/client_stub.o
	$(CC) $^ -o $@

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

sdmessage.pb-c.o:
	protoc --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c source
	mv sdmessage.pb-c.h include
	$(CC) $(CFLAGS) -c $(SRC_DIR)/sdmessage.pb-c.c -o $(OBJ_DIR)/sdmessage.pb-c.o

client_stub.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client_stub.c -o $(OBJ_DIR)/client_stub.o

table_client.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/table_client.c -o $(OBJ_DIR)/table_client.o

libtable: data.o entry.o list.o table.o
	ar -rcs $(LIB_DIR)/libtable.a $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o $(OBJ_DIR)/table.o

table_client: libtable client_stub.o sdmessage.pb-c.o
	$(CC) $(CFLAGS) $(SRC_DIR)/table_client.c -o $(BIN_DIR)/table_client $(LIB_DIR)/libtable.a $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/sdmessage.pb-c.o -I/usr/include/ -L/usr/include -lprotobuf-c

client_run: table_client
	./$(BINDIR)/table_client 127.0.0.1:1337

cclient_valgrind: tree_client
	valgrind --leak-check=full --track-origins=yes $(BINDIR)/tree_client 127.0.0.1:1337 < ./tests/del01.txt



data.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/data.c -o $(OBJ_DIR)/data.o
entry.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/entry.c -o $(OBJ_DIR)/entry.o
list.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o
table.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/table.c -o $(OBJ_DIR)/table.o
