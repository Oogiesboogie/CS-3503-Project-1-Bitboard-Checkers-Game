# ================================
# Makefile for BitBoard Checkers
# ================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Project files
SRC = main.c checkers_bitboard.c
OBJ = $(SRC:.c=.o)
TARGET = checkers

# Default target (what runs when you type "make")
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to compile .c to .o
%.o: %.c checkers_bitboard.h
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Remove compiled files
clean:
	rm -f $(OBJ) $(TARGET)

# Optional: rebuild everything from scratch
rebuild: clean all