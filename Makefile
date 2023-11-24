CC = clang
CFLAGS = -Wall -Wextra -std=c99
DEBUG_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer
SRC_DIR = src
BUILD_DIR = bin
LOG_DIR = diagnostics

# Colors
RED = \033[0;31m
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
NC = \033[0m

# Find all source files in tp* directories
SOURCES = $(wildcard $(SRC_DIR)/tp*/*.c)

# Generate executable names
EXECUTABLES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%, $(SOURCES))

.PHONY: all debug clean

all: $(EXECUTABLES)
	@bear -- $(MAKE) $(EXECUTABLES) > /dev/null 2>&1

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(EXECUTABLES)

$(BUILD_DIR)/%:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LOG_DIR)
	@dir_name=$(shell dirname $*); \
	base_name=$(shell basename $*); \
	src_file=$(SRC_DIR)/$$dir_name/$$base_name.c; \
	exec_file=$(BUILD_DIR)/$$dir_name$$base_name; \
	log_file=$(LOG_DIR)/$$dir_name$$base_name.log; \
	if [ -f $$src_file ]; then \
		echo "$(BLUE)Compiling $$src_file to $$exec_file$(NC)"; \
		$(CC) $(CFLAGS) $$src_file -o $$exec_file 2> $$log_file; \
		if [ $$? -ne 0 ] || [ -s $$log_file ]; then \
			echo "$(RED)Compilation failed: $$exec_file. Check $$log_file for details.$(NC)"; \
		else \
			rm -f $$log_file; \
		fi \
	else \
		echo "$(YELLOW)Warning: Source file for $$exec_file not found. Skipping.$(NC)"; \
	fi

clean:
	@echo "$(YELLOW)Cleaning build directory...$(NC)"
	@rm -rf $(BUILD_DIR) $(LOG_DIR)
	@echo "$(YELLOW)Clean complete.$(NC)"