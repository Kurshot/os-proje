# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files and target files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/command.c $(SRC_DIR)/io_redirect.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET = $(BIN_DIR)/shell
INCREMENT = increment

# Default target
all: $(TARGET) $(INCREMENT)

# Build the shell executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "Shell executable built successfully!"

# Build the increment executable
$(INCREMENT): $(SRC_DIR)/increment.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Increment executable built successfully!"

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(INCREMENT)
	@echo "Cleaned up all build files!"

