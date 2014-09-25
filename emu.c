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

  for(u16 i = 0; i != 0xffff; i++) {
    if(i % 8 == 0 && i) {
      printf("\n");
    }
    if(i % 8 == 0) {
      printf("%04x: ", i);
    }
    printf("%04x ", memory[i]);
  }
  printf("\n");
}


u16 *oper_values(u16 word, bool is_a) {
  if (word > 0x3f){
    printf("no good\n");
    exit(0);
  }

  u16 next_word;

  if (word <= 0x07) {
    return &(operands[word]);
  }

  if (word <= 0x0f) {
    return &(memory[operands[word - 0x08]]);
  }

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
    u16 val = word - 0x21;
    u16 *lit = malloc(sizeof(u16));
    lit = &val;
    return lit;
  }

  printf("Invalid instruction");
  exit(0);
}

/////

u16 num_skips(u16 word) {
  return ((word >= 8 && word <= 17) || word == 0x1a || word == 0x1e || word == 0x1f);
}

u16 instr_len(u16 word) {
  u16 b_bits = (0x03e0 & word) >> 5;
  u16 a_bits = (0xfc00 & word) >> 10;

  return num_skips(b_bits) + num_skips(a_bits) + 1;
}

void skip_instr() {
  u16 len_this_instr = instr_len(memory[operands[PC]]);
  u16 len_next_instr = instr_len(memory[operands[PC] + len_this_instr]);
  operands[PC] += len_next_instr;
}

/////

void process(u16 word) {
  u16 o_bits = (0x001f & word);
  u16 b_bits = (0x03e0 & word) >> 5;
  u16 a_bits = (0xfc00 & word) >> 10;

  u16 *b = oper_values(b_bits, false);
  u16 *a = oper_values(a_bits, true);


  switch(o_bits) {
    case NA:
      break;
    case SET:
      *b = *a;
      break;
    case ADD:
      *b += *a;
      if (*b < *a)
        operands[EX] = 0x0001;
      else
        operands[EX] = 0x0000;
      break;
    case SUB:
      *b -= *a;
      if (*b > *a)
        operands[EX] = 0x0001;
      else
        operands[EX] = 0x0000;
      break;
    case MUL:
      *b *= *a;
      operands[EX] = ((*b * *a) >> 16) & 0xffff;
      break;
    case MLI:
      *b = (int16_t) *a * (int16_t) *b;
      break;
    case DIV:
      if (*a == 0) {
        *b = 0;
        operands[EX] = 0;
      }
      else {
        *b /= *a;
        operands[EX] = ((*b << 16) / *a) & 0xffff;
      }
      break;
    case DVI:
      if (*a == 0) {
        *b = 0;
        operands[EX] = 0;
      }
      else {
        *b = (int16_t) *b / (int16_t) *a;
      }
      break;
    case MOD:
      if (*a == 0)
        *b = 0;
      else
        *b %= *a;
      break;
    case MDI:
      if (*a == 0)
        *b = 0;
      else
        *b = (int16_t) *b % (int16_t) *a;
      break;
    case AND:
      *b &= *a;
      break;
    case BOR:
      *b |= *a;
      break;
    case XOR:
      *b ^= *a;
      break;
    case SHR:
      *b >>= *a;
      operands[EX] = ((*b << 16) >> *a) && 0xffff;
      break;
    case ASR:
      *b = (int16_t) *b >> *a;
      operands[EX] = ((*b << 16) >> *a) && 0xffff;
      break;
    case SHL:
      *b <<= *a;
      operands[EX] = ((*b << *a) >> 16) && 0xffff;
      break;
    case IFB:
      if (*a & *b == 0)
        skip_instr();
      break;
    case IFC:
      if (*a & *b != 0)
        skip_instr();
      break;
    case IFE:
      if (*a != *b)
        skip_instr();
      break;
    case IFN:
      if (*a == *b)
        skip_instr();
      break;
    case IFG:
      if (*a <= *b)
        skip_instr();
      break;
    case IFA:
      if ((int16_t) *a <= (int16_t)*b)
        skip_instr();
      break;
    case IFL:
      if (*a >= *b)
        skip_instr();
      break;
    case IFU:
      if ((int16_t) *a >= (int16_t)*b)
        skip_instr();
      break;
    case ADX:
      *b = *b + *a + operands[EX];
      if (*b < (*a + operands[EX]))
        operands[EX] = 0x0001;
      else
        operands[EX] = 0x0000; 
      break;
    case SBX:
      *b = *b - *a + operands[EX];
      if (*a < *b)
        operands[EX] = 0xffff;
      else
        operands[EX] = 0x0000; 
      break;
    case STI:
      *b = *a;
      operands[I]++;
      operands[J]++;
      break;
    case STD:
      *b = *a;
      operands[I]--;
      operands[J]--;
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
