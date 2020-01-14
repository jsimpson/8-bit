CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wmissing-prototypes -pedantic -g -O3 -std=c99

all: disassembler-8080 disassembler-8080-library

disassembler-8080:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE main.c 8080/disassembler.c -o build/disassembler-8080 $^

disassembler-8080-library:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE -shared 8080/disassembler.c -o build/libdisassembler-8080.so $^

clean:
	rm build/disassembler-8080
	rm build/libdisassembler-8080.so
