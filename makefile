# The compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall -Werror -Wextra -ansi -pedantic

# Include folder
INCLUDE = include

# Source folder
SRC = src

# Binary folder
BIN = bin

# Object files
OBJ = $(BIN)/main.o $(BIN)/prompt.o $(BIN)/parse.o $(BIN)/execute.o

# Object dependents
DMAIN		= $(BIN)/prompt.o $(BIN)/parse.o $(BIN)/execute.o $(SRC)/main.c
DPROMPT 	= $(INCLUDE)/shell_consts.h $(INCLUDE)/prompt.h $(SRC)/prompt.c
DPARSE		= $(INCLUDE)/command.h $(INCLUDE)/shell_consts.h $(INCLUDE)/parse.h $(SRC)/parse.c
DEXECUTE	= $(INCLUDE)/command.h $(INCLUDE)/shell_consts.h $(INCLUDE)/execute.h $(SRC)/execute.c

# Object commands
CMAIN		= -c $(SRC)/main.c
CPROMPT		= -D_DEFAULT_SOURCE -c $(SRC)/prompt.c
CPARSE		= -D_DEFAULT_SOURCE -c $(SRC)/parse.c
CEXECUTE	= -c $(SRC)/execute.c

# Target executable
TARGET = $(BIN)/AP-shell

all: $(TARGET)

$(TARGET): $(BIN) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BIN)/main.o: $(DMAIN)
	$(CC) $(CFLAGS) -o $(BIN)/main.o $(CMAIN)

$(BIN)/prompt.o: $(DPROMPT)
	$(CC) $(CFLAGS) -o $(BIN)/prompt.o $(CPROMPT)

$(BIN)/parse.o: $(DPARSE)
	$(CC) $(CFLAGS) -o $(BIN)/parse.o $(CPARSE)

$(BIN)/execute.o: $(DEXECUTE)
	$(CC) $(CFLAGS) -o $(BIN)/execute.o $(CEXECUTE)

$(BIN):
	mkdir -p $(BIN)

clean:
	$(RM) -r $(BIN)
