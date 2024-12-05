# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude

# Sources, objects, and targets
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/command.c $(SRC_DIR)/io_redirect.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
TARGET = shell
INCREMENT = increment

# Default target
all: $(TARGET) $(INCREMENT)

# Build the main shell executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Build the increment executable
$(INCREMENT): $(SRC_DIR)/increment.c
	$(CC) $(CFLAGS) $< -o $@

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(INCREMENT)
