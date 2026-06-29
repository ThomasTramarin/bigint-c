CC=gcc
FLAGS=-Wall -Wextra

SRC=src/bigint.c
TEST=test/main.c

BUILD_DIR=build
BIN=$(BUILD_DIR)/test

all: $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN): $(SRC) $(TEST) | $(BUILD_DIR)
	$(CC) $(FLAGS) $(TEST) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)