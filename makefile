# Grupo 21
# Joao Santos 56380
# Rafael Ferreira 57544
# Ricardo Mateus 56366

# Diretório de objetos
OBJ_DIR = object
INC_DIR = include
LIBINCLUDEDIR = /usr/include/zookeeper
LIB_DIR = lib
BIN_DIR = binary
SRC_DIR = source
DEP_DIR = dependencies

CC = gcc
CFLAGS = -Wall -g -I $(INC_DIR) -I $(LIBINCLUDEDIR)
LDFLAGS = -L/usr/include -lprotobuf-c -lpthread -lm -lzookeeper_mt
AR = ar
ARFLAGS = rcs

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executables
EXECS = table-server table-client
TESTS = test_data test_entry test_list test_serialization test_table

.PHONY: all clean build run

all: build $(EXECS) $(addprefix $(BIN_DIR)/,$(EXECS))

build:
	mkdir -p $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR) $(DEP_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	$(CC) $^ -o $@ $(LDFLAGS)

$(LIB_DIR)/libtable.a: $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o $(OBJ_DIR)/table.o
	$(AR) $(ARFLAGS) $@ $^

table-server: $(LIB_DIR)/libtable.a $(OBJ_DIR)/sdmessage.pb-c.o $(OBJ_DIR)/network_client.o $(OBJ_DIR)/network_server.o $(OBJ_DIR)/table_skel.o $(OBJ_DIR)/message-private.o $(OBJ_DIR)/client_zookeeper.o $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/bubble_sort.o
	$(CC) $(CFLAGS) $(SRC_DIR)/zookeeper.c $(SRC_DIR)/table_server.c -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

table-client: $(LIB_DIR)/libtable.a $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/sdmessage.pb-c.o $(OBJ_DIR)/message-private.o $(OBJ_DIR)/network_client.o $(OBJ_DIR)/client_zookeeper.o $(OBJ_DIR)/bubble_sort.o
	$(CC) $(CFLAGS) $(SRC_DIR)/table_client.c -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

zks_server_run: table-server
	./$(BIN_DIR)/table-server 1337 4 172.17.254.246:2181

zks_server_run2: table-server
	./$(BIN_DIR)/table-server 1338 4 172.17.254.246:2181

zks_server_run3: table-server
	./$(BIN_DIR)/table-server 1339 4 172.17.254.246:2181

zks_server_run3: table-server
	./$(BIN_DIR)/table-server 1340 4 172.17.254.246:2181

zkc_client_run: table-client
	./$(BIN_DIR)/table-client 172.17.254.246:2181

run:
	$(addprefix $(BIN_DIR)/,$(EXECS))

clean:
	rm -rf $(DEP_DIR)/* $(OBJ_DIR)/* $(BIN_DIR)/* $(LIB_DIR)/*
