CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wmissing-prototypes -pedantic -g -O3 -std=c99
LDFLAGS=-lm

all: disassembler-8080

disassembler-8080:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE main.c 8080/disassembler.c -o disassembler-8080 $^ $(LDFLAGS)

clean:
	rm disassembler-8080
