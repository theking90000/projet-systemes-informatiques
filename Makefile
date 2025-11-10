CC       = gcc
CFLAGS   = -Wall -Wextra -Iinclude
LDFLAGS  = -lwiringPi

SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin

SRC      = $(wildcard $(SRC_DIR)/*.c)
OBJ      = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET   = $(BIN_DIR)/math_suite

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: run
run: clean all
	./math_suite
