CC = clang
CFLAGS = -Wall -Wextra -std=c99
DEBUG_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer
SRC_DIR = src
BUILD_DIR = build

# Colors
RED = \033[0;31m
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
NC = \033[0m # No Color

# Find all source files
SOURCES = $(wildcard $(SRC_DIR)/tp0/*.c $(SRC_DIR)/tp1/*.c $(SRC_DIR)/tp2/*.c)
# Generate executable names
EXECUTABLES = $(foreach src,$(SOURCES),$(BUILD_DIR)/$(shell basename $(dir $(src)) )$(notdir $(basename $(src))))

.PHONY: all debug clean

all: $(EXECUTABLES)
	@bear -- $(MAKE) $(EXECUTABLES)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(EXECUTABLES)

$(BUILD_DIR)/%: 
	@mkdir -p $(BUILD_DIR)
	@$(if $(findstring debug,$@), \
		echo "$(GREEN)Debug compiling $@$(NC)", \
		echo "$(BLUE)Compiling $@$(NC)")
	@$(CC) $(CFLAGS) $(SRC_DIR)/$(shell echo $* | sed -e 's/^\(tp[0-9]\)\(.*\)/\1\/\2.c/') -o $@ || echo "$(RED)Compilation failed: $@$(NC)"

clean:
	@echo "$(YELLOW)Cleaning build directory...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(YELLOW)Clean complete.$(NC)"
