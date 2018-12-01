CC=gcc
CFLAGS=-pthread

server: main.o
	$(CC) -g main.cpp -o server

clean:
	rm main.o server