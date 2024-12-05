CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/main.c src/shell.c src/command.c src/io_redirect.c
OBJ = $(SRC:.c=.o)
TARGET = shell
INCREMENT = increment

all: $(TARGET) $(INCREMENT)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

$(INCREMENT): src/increment.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(INCREMENT)
