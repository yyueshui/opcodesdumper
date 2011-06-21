# opcodes_dumper Makefile 
# laruence <laruence at gmail.com>

include Config

CC = gcc

CFLAGS  = $(shell $(PHP_CONFIG) --includes)
LDFLAGS = -L$(shell $(PHP_CONFIG) --prefix)/lib -lstdc++ -lphp5

all:
	$(CC) -o opcodes_dumper opcodes_dumper.cpp $(CFLAGS) $(LDFLAGS)
clean:
	rm -rf opcodes_dumper 
