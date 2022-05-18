# The compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall -Werror -ansi -pedantic

# Include folder
INCLUDE = include

# Source folder
SRC = src

# Binary folder
BIN = bin

# Object files
OBJ = $(BIN)/ap-shell.o

# Target executable
TARGET = $(BIN)/AP-shell

all: $(TARGET)

$(TARGET): $(BIN) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BIN)/ap-shell.o: $(INCLUDE)/ap-shell.h $(SRC)/ap-shell.c
	$(CC) $(CFLAGS) -D_XOPEN_SOURCE=500 -o $(BIN)/ap-shell.o -c $(SRC)/ap-shell.c

$(BIN):
	mkdir -p $(BIN)

clean:
	$(RM) -r $(BIN)
