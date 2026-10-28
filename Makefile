CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Idata_types/include

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = $(SRC_DIR)/main.c
OBJ = $(OBJ_DIR)/main.o
TARGET = $(BIN_DIR)/test_runner

# Default target
all: directories $(TARGET)

# Ensure directories exist before building
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Compile source file into an object file in /obj
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link the object file into the executable in /bin
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Run the compiled test binary
run: all
	./$(TARGET)

# Clean up built binaries and object files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories run clean
