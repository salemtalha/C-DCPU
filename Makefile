CC=gcc
CFLAGS=-std=c99

all: asm emu

asm: 
	$(CC) $(CFLAGS) dcpu.c -o asm

emu:
	$(CC) $(CFLAGS) emu.c -o emu 
