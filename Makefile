# MIPS Test Makefile

CC = gcc
SRC = MIPS_Test.c MIPS_32.c shell.c
OBJ = MIPS_Test.o MIPS_32.o shell.o
EXE = MIPS_Test.exe
VPATH = src ../Lab_01/ ../Lab_01/src/
CFLAGS = -Wall -g

all: obj exec

obj: $(SRC)
	@echo "Compiling object files.."
	$(CC) $(CFLAGS) -c $^
	
exec: $(OBJ)
	@echo "Compiling executable..."
	$(CC) $(CFLAGS) -o $(EXE) $^ 

.PHONY: clean
clean:
	@echo "Removing exe and obj files..."
	rm -f $(OBJ) $(EXE)
	@echo "Removed files successfully"