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
OBJ = $(BIN)/main.o $(BIN)/prompt.o $(BIN)/parse.o $(BIN)/execute.o

# Target executable
TARGET = $(BIN)/AP-shell

all: $(TARGET)

$(TARGET): $(BIN) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BIN)/main.o: $(BIN)/prompt.o $(BIN)/parse.o $(BIN)/execute.o $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(BIN)/main.o -c $(SRC)/main.c

$(BIN)/prompt.o: $(INCLUDE)/prompt.h $(SRC)/prompt.c
	$(CC) $(CFLAGS) -D_XOPEN_SOURCE=500 -o $(BIN)/prompt.o -c $(SRC)/prompt.c

$(BIN)/execute.o: $(BIN)/parse.o $(INCLUDE)/execute.h $(SRC)/execute.c
	$(CC) $(CFLAGS) -o $(BIN)/execute.o -c $(SRC)/execute.c

$(BIN)/parse.o: $(INCLUDE)/parse.h $(SRC)/parse.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -o $(BIN)/parse.o -c $(SRC)/parse.c

$(BIN):
	mkdir -p $(BIN)

clean:
	$(RM) -r $(BIN)
