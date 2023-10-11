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

EXECS = $(BIN_DIR)/test_data $(BIN_DIR)/test_entry $(BIN_DIR)/test_list $(BIN_DIR)/test_table $(BIN_DIR)/test_serialization

make: $(EXECS)

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


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm -rf $(DEP_DIR)/* $(OBJ_DIR)/* $(BIN_DIR)/*

build:
	mkdir $(OBJ_DIR)
	mkdir $(BIN_DIR)

valgrind_all:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_data
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_entry
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_list
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_table
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_serialization

valgrind_data:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_data
valgrind_entry:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_entry
valgrind_list:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_list
valgrind_table:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_table
valgrind_serialization:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(BIN_DIR)/test_serialization

sdmessage.pb-c.o:
	protoc --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c source
	mv sdmessage.pb-c.h include
	$(CC) $(CFLAGS) -c $(SRC_DIR)/sdmessage.pb-c.c -o $(OBJ_DIR)/sdmessage.pb-c.o
