CC       = gcc
CFLAGS   = -Wall -Wextra
LDFLAGS  = -lwiringPi
OBJFILES = color.o led.o main.o
TARGET   = main

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJFILES) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET)

.PHONY: run
run: clean all
	./main
