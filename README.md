This is an assembler for DCPU 1.7. I plan on adding an emulator when I find some time. 

This follows the spec modulo the hardware specific stuff. I'm sure there are bugs and I will find them as time goes on. 
I make a lot of assumptions since I'm writing in C and don't wanna spend forever handing arbitrary length strings etc.

usage:

gcc -std=c99 dcpu.c
./a.out < asm.txt
