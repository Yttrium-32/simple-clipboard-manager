SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

TARGET := $(BUILD_DIR)/clipboard_manager
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CFLAGS := -Wall -Wextra -lX11

.PHONY: run all clean cache

all: $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJ) | $(BUILD_DIR)
	@echo "Building target..."
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Successfully build target at $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BUILD_DIR) $(OBJ_DIR)
	
cache: clean
	@echo "Generating compilation cache..."
	bear -- make all
	@echo "Compilation cache generated successfully!"

