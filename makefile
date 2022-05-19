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
OBJ = $(BIN)/main.o $(BIN)/prompt.o

# Target executable
TARGET = $(BIN)/AP-shell

all: $(TARGET)

$(TARGET): $(BIN) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BIN)/main.o: $(SRC)/main.c $(BIN)/prompt.o $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(BIN)/main.o -c $(SRC)/main.c

$(BIN)/prompt.o: $(INCLUDE)/prompt.h $(SRC)/prompt.c
	$(CC) $(CFLAGS) -D_XOPEN_SOURCE=500 -o $(BIN)/prompt.o -c $(SRC)/prompt.c

$(BIN):
	mkdir -p $(BIN)

clean:
	$(RM) -r $(BIN)
