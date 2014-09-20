#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef int16_t u16;

u16 memory[0x10000] = { 0 };
u16 registers[8] = { 0 };
u16 pcounter = 0;
u16 spointer = 0;

enum regs {
  A, B, C, X, Y, Z, I, J 
}; 

enum ops {
  NA, SET, ADD, SUB, MUL, MLI, DIV, DVI, MOD,
  MDI, AND, BOR, XOR, SHR, ASR, SHL, IFB,
  IFC, IFE, IFN, IFG, IFA, IFL, IFU, U1,
  U2, ADX, SBX, U3, U4, STI, STD
};

void write_word(u16 value) {
  memory[spointer] = value;
  spointer++;
}

void dump_mem() {
  for(u16 i = 0; i < 10000; i++) {
    if(i % 8 == 0 && i) {
      printf("\n");
    }
    if (!memory[i]){
      printf("\n");
      return;
    }
    printf("%04x ", memory[i]);
  }
}
