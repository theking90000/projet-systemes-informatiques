main: clean
	cc color.c led.c main.c -o main -lwiringPi -Wall -Wextra
	chmod +x main

all: led

clean:
ifeq ($(wildcard main), main)
	rm main
endif