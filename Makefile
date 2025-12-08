CC       = gcc
CFLAGS   = -Wall -Wextra -Iinclude
LDFLAGS  = -lwiringPi

SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin

SRC      = $(wildcard $(SRC_DIR)/*.c)
OBJ      = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET   = $(BIN_DIR)/math_suite

NO_LED ?= 0
THREAD ?= 0
FORK   ?= 0
DEBUG  ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -O0
else
    CFLAGS += -O3
endif

FORK ?= 0
ifeq ($(FORK), 1)
    CFLAGS += -DUSE_FORK
    THREAD = 0
endif

THREAD ?= 0
ifeq ($(THREAD), 1)
    CFLAGS += -DUSE_THREAD -lpthread
endif

ifeq ($(NO_LED), 1)
    CFLAGS += -DNO_LED
endif

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
	./bin/math_suite --input input.txt
