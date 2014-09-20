#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint16_t u16;

u16 memory[0x10000] = { 0 };
u16 operands[12] = { 0 };

enum operands {
  A, B, C, X, Y, Z, I, J, PC, SP, EX, IA
}; 

enum ops {
  NA, SET, ADD, SUB, MUL, MLI, DIV, DVI, MOD,
  MDI, AND, BOR, XOR, SHR, ASR, SHL, IFB,
  IFC, IFE, IFN, IFG, IFA, IFL, IFU, U1,
  U2, ADX, SBX, U3, U4, STI, STD
};

void write_word(u16 value) {
  memory[operands[PC]] = value;
  operands[PC]++;
}

void dump_mem() {
  printf("PC: %04x\tSP: %04x\tIA: %04x\tEX: %04x\n", operands[PC], operands[PC], operands[IA], operands[EX]);
  printf("A: %04x\tB: %04x\tC: %04x\n", operands[A], operands[B], operands[C]);
  printf("X: %04x\tY: %04x\tZ: %04x\n", operands[X], operands[Y], operands[Z]);
  printf("I: %04x\tJ: %04x\n", operands[I], operands[J]);

  printf("\n\n");

  for(u16 i = 0; i <= 0xffff; i++) {
    if(i % 8 == 0 && i) {
      printf("\n");
    }
    if(i % 9 == 0) {
      printf("%04x: ", i);
    }
    if (!memory[i]){
      printf("\n");
      return;
    }
    printf("%04x ", memory[i]);
  }
}


u16 *oper_values(u16 word, bool is_a) {
  if (word > 0x3f){
    printf("no good\n");
    exit(0);
  }

  u16 next_word;

  if (word <= 0x07)
    return &(operands[word]);

  if (word <= 0x0f)
    return &(memory[operands[word - 0x08]]);

  if (word <= 0x17) {
    operands[PC]++;
    next_word = memory[PC];
    return &(memory[operands[word - 0x10] + next_word]);
  }

  if (word == 0x18) {
    if (is_a)
      return &(memory[--operands[SP]]);
    else
      return &(memory[operands[SP]++]);
  }

  if (word == 0x19)
    return &(memory[operands[SP]]);

  if (word == 0x1a) {
    operands[PC]++;
    next_word = memory[PC];
    return &(memory[operands[SP]+ next_word]);
  }

  if (word == 0x1b)
    return &(operands[SP]);

  if (word == 0x1c)
    return &(operands[PC]);

  if (word == 0x1d)
    return &(operands[EX]);

  if (word == 0x1e) {
    operands[PC]++;
    next_word = memory[PC];
    return &(memory[next_word]);
  }

  if (word == 0x1f) {
    if (is_a) {
      operands[PC]++;
      next_word = memory[PC];
      u16 *next_word_c = malloc(sizeof(u16));
      next_word_c = &next_word;
      return next_word_c;
    }
    return NULL;
  }

  if(word >= 0x20 && word <= 0x3f && is_a) {
    word -= 0x21;
    u16 *lit = malloc(sizeof(u16));
    lit = &word;
    return lit;
  }

}

void process(u16 word) {
  u16 o = (0x001f & word);
  u16 b = (0x03e0 & word) >> 5;
  u16 a = (0xfc00 & word) >> 10;

  u16 *oper1 = oper_values(b, false);
  u16 *oper2 = oper_values(a, true);

  switch(o) {
    case NA:
      break;
    case SET:
      *oper1 = *oper2;
      break;
    case ADD:
      *oper1 += *oper2;
      if (*oper1 < *oper2)
        operands[EX] = 0x0001;
      else
        operands[EX] = 0x0000;
      break;
    case SUB:
      *oper1 -= *oper2;
      break;
    case MUL:
      break;
    case MLI:
      break;
    case DIV:
      break;
    case DVI:
      break;
    case MOD:
      break;
    case MDI:
      break;
    case AND:
      break;
    case BOR:
      break;
    case XOR:
      break;
    case SHR:
      break;
    case ASR:
      break;
    case SHL:
      break;
    case IFB:
      break;
    case IFC:
      break;
    case IFE:
      break;
    case IFN:
      break;
    case IFG:
      break;
    case IFA:
      break;
    case IFL:
      break;
    case IFU:
      break;
    case ADX:
      break;
    case SBX:
      break;
    case STI:
      break;
    case STD:
      break;
  }

  operands[PC]++;
}


void run() {
  while(operands[PC] != 0xffff) {
    process(memory[operands[PC]]);
  }
}

int main() {
  u16 word = 0;
  while(fread(&word, 2, 1, stdin)) {
    write_word(word);
  }

  operands[SP] = 0xffff;
  operands[PC] = 0;

  run();
  dump_mem();
}
