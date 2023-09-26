# Grupo 21
# Joao Santos 56380
# Rafael Ferreira 57544
# Ricardo Mateus

# Diretório de objetos
OBJ_DIR = object
INC_DIR = include
LIB_DIR = lib
BIN_DIR = binary
SRC_DIR = source
DEP_DIR = dependencies

CC = gcc

CFLAGS = -Wall -g -I $(INC_DIR)

EXECS = $(BIN_DIR)/test_data $(BIN_DIR)/test_entry $(BIN_DIR)/test_list $(BIN_DIR)/test_table #$(BIN_DIR)/test_serialization

make: $(EXECS)

run:
	$(BIN_DIR)/test_data
	$(BIN_DIR)/test_entry
	$(BIN_DIR)/test_list
#	$(BIN_DIR)/test_serialization
	$(BIN_DIR)/test_table

$(BIN_DIR)/test_data: $(OBJ_DIR)/test_data.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_entry: $(OBJ_DIR)/test_entry.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_list: $(OBJ_DIR)/test_list.o $(OBJ_DIR)/list.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_serialization: $(OBJ_DIR)/test_serialization.o $(OBJ_DIR)/serialization.o $(OBJ_DIR)/list.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@

$(BIN_DIR)/test_table: $(OBJ_DIR)/test_table.o $(OBJ_DIR)/table.o $(OBJ_DIR)/list.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/data.o
	$(CC) $^ -o $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm -rf $(DEP_DIR)/* $(OBJ_DIR)/* $(BIN_DIR)/*
