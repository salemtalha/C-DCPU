#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef int16_t u16;

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
  memory[operands[SP] = value];
  operands[SP]++;
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


//u16 oper_values(u16 word) {
//  if (word > 0x3f){
//    printf("no good\n");
//    exit(0);
//  }
//
//  u16 next_word;
//
//  if (word <= 0x07)
//    return word;
//  if (word <= 0x0f)
//    return operands[word - 0x08];
//  if (word <= 0x17)
//    fread(&next_word, 2, 1, stdin);
//    return operands[operands[word] + next_word];
//  if (word == 0x18)
//    return 0;
//  if (word == 0x19)
//    return memory[operands[SP]];
//  if (word == 0x1a)
//    fread(&next_word, 2, 1, stdin);
//    return memory[operands[SP]+ next_word];
//  if (word == 0x1b)
//    return operands[SP];
//}

void process(u16 word) {
  u16 arg_o = (0x001f & word);
  u16 arg_b = (0x03e0 & word) >> 5;
  u16 arg_a = (0xfc00 & word) >> 10;


  switch(arg_o) {
    case SET:
      break;
    case ADD:
      break;
    case SUB:
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
    default:
      return;
  }

  printf("%04x %04x %04x\n", arg_a, arg_b, arg_o);  
  exit(0);

}

void run() {

}

int main() {
  u16 word = 0;

  while(fread(&word, 2, 1, stdin)) {
    //process(word);
    write_word(word);
  }

  run();
  //dump_mem();
}
