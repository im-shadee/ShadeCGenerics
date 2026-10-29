CC = gcc
# Remove optimization from global CFLAGS so it can be controlled per file
CFLAGS = -Wall -Wextra -std=c99 -Idata_types/include

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET_MAIN = $(BIN_DIR)/test_runner
TARGET_BENCH = $(BIN_DIR)/benchmark

# Default target builds both executables
all: directories $(TARGET_MAIN) $(TARGET_BENCH)

# Ensure directories exist before building
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Default optimization level for any file that doesn't specify one
OPT_LEVEL ?= -O2

# Generic pattern rule to compile any .c file in src/ into obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(OPT_LEVEL) -c $< -o $@

# Compile main.c with -Os (optimize for size)
$(OBJ_DIR)/main.o: OPT_LEVEL = -Os

# Compile benchmark.c with -O3 (maximum performance)
$(OBJ_DIR)/benchmark.o: OPT_LEVEL = -O3

# Link main.o into the test runner executable
$(TARGET_MAIN): $(OBJ_DIR)/main.o
	$(CC) $< -o $@

# Link benchmark.o into the benchmark executable
$(TARGET_BENCH): $(OBJ_DIR)/benchmark.o
	$(CC) $< -o $@

# Run the compiled test binary
run: all
	./$(TARGET_MAIN)

# Run the compiled benchmark binary
benchmark: all
	./$(TARGET_BENCH)

# Clean up built binaries and object files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories run benchmark clean
