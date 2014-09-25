This is an assembler and emulator for DCPU 1.7.

This follows the spec modulo the hardware specific stuff. There are bugs and I will find them as time goes on. 
I make a lot of assumptions since I'm writing in C and don't wanna spend forever handing arbitrary length strings etc.

Usage:

    make
  
    ./asm < foo.dasm > foo.bin
  
    ./emu < foo.bin
