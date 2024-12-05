# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Files
SHELL_TARGET = $(BIN_DIR)/shell

SHELL_SRC_FILES = $(filter-out $(SRC_DIR)/increment.c, $(wildcard $(SRC_DIR)/*.c))
SHELL_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SHELL_SRC_FILES))
INCREMENT_OBJ = $(OBJ_DIR)/increment.o

# Default target
all: $(SHELL_TARGET)

# Build the main shell program
$(SHELL_TARGET): $(SHELL_OBJ_FILES) $(INCREMENT_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Rebuild everything
rebuild: clean all