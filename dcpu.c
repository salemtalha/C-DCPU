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

static const char *regs_str = "ABCXYZIJ";

enum ops {
  NA, SET, ADD, SUB, MUL, MLI, DIV, DVI, MOD,
  MDI, AND, BOR, XOR, SHR, ASR, SHL, IFB,
  IFC, IFE, IFN, IFG, IFA, IFL, IFU, U1,
  U2, ADX, SBX, U3, U4, STI, STD
};

static const char *ops_strs[] = {
  "NA", "SET", "ADD", "SUB", "MUL", "MLI", "DIV", "DVI", "MOD",
  "MDI", "AND", "BOR", "XOR", "SHR", "ASR", "SHL", "IFB",
  "IFC", "IFE", "IFN", "IFG", "IFA", "IFL", "IFU", "U1",
  "U2", "ADX", "SBX", "U3", "U4", "STI", "STD"
};

enum ops find_op_num(char *op) {
  for(int i = 0; i < 32; i++)
    if (!strcmp(op, ops_strs[i]))
      return i;

  return -1;
}

int find_reg_num(char *arg) {
  char *match;
  match = strstr(regs_str, arg);
  return match == NULL ? -1 : match - regs_str;
}

char **split(char buf[], char *pattern) {
  char **tokens = malloc(sizeof(char*) * 3);
  memset(tokens, 0, sizeof(char *) * 3);

  char *token;

  token = strtok(buf, pattern);
  tokens[0] = token;
  token = strtok(NULL, pattern);
  tokens[1] = token;
  token = strtok(NULL, pattern);
  tokens[2] = token;

  return tokens;
}

bool convert_to_base(char *token, int base, u16 *literal) {
  char *end;
  *literal = strtol(token, &end, base);
  if (!*end)
    return true;
  else
    return false;
}

bool is_literal(char *token, u16 *literal) {
  bool can_convert_hex = convert_to_base(token, 16, literal);
  if (can_convert_hex)
    return true;
  bool can_convert_dec = convert_to_base(token, 10, literal);
  if (can_convert_dec)
    return true;

  return false;
}


u16 get_instr_value(char *token, bool is_a) {
  u16 reg_num = find_reg_num(token);
  if (reg_num > -1)
    return reg_num;
  if (!strcmp(token, "SP"))
    return 0x1b;
  if (!strcmp(token, "PC"))
    return 0x1c;
  if (!strcmp(token, "EX"))
    return 0x1d;
  if (!strcmp(token, "PUSH") || !strcmp(token, "POP"))
    return 0x18;

  u16 lit;
  if (is_literal(token, &lit)) { 
    if (lit >= -1 && lit <= 30 && is_a)
      return lit;
    return 0x1f;
  }

  char **parts = split(token, "+[]");

  //register / [register + lit]
  if ((reg_num = find_reg_num(parts[0])) > -1)
    if (parts[1] && is_literal(parts[1], &lit))
      return 0x10 + reg_num; 
    else
      return 0x08 + reg_num;

  // SP / [SP + lit]
  if(parts[0] == "SP")
    if (parts[1] && is_literal(parts[1], &lit))
      return 0x1a;
    else
      return 0x19;

  // [lit]
  if (is_literal(parts[0], &lit))
    return 0x1e; 

  free(parts);
  return -1;
}

void write_word(u16 value) {
  memory[spointer] = value;
  spointer++;
}

void write_operand(char *token, bool is_a) {
  u16 reg_val = find_reg_num(token);
  if (reg_val > - 1) {
    return;
  }

  u16 lit;

  if (is_literal(token, &lit)) { 
    if (lit >= -1 && lit <= 30 && is_a) {
      return;
    }
    write_word(lit);
    return;
  }

  char **parts = split(token, "+[]");

  //register / [register + lit]
  if ((reg_val = find_reg_num(parts[0])) > -1) {
    if (parts[1] && is_literal(parts[1], &lit)) {
      write_word(registers[reg_val] + lit); 
      return;
    }
    write_word(memory[registers[reg_val]]);
    return;
  }

  // SP / [SP + lit]
  if(parts[0] == "SP") {
    if (parts[1] && is_literal(parts[1], &lit)) {
      write_word(memory[spointer + lit]);
      return;
    }
    write_word(memory[spointer]);
    return;
  }

  // [lit]
  if (is_literal(parts[0], &lit))
    write_word(lit); 
    return;

  free(parts);
}


u16 get_high_bits(char **instr, enum ops op) {
  u16 base = op;
  base |= (get_instr_value(instr[2], true) << 10);
  base |= (get_instr_value(instr[1], false) << 5);

  return base;
}

void assemble(char buf[]) {
  char *cpy = malloc(sizeof(char) * 256);
  strcpy(cpy, buf);

  char **instr = split(buf, " ,\n");

  char *op = instr[0];
  u16 high_bits, middle_bits, low_bits;

  enum ops op_num = find_op_num(op);
  high_bits = get_high_bits(instr, op_num);

  write_word(high_bits);

  char **cpy_instr = split(cpy, " ,\n");

  write_operand(cpy_instr[2], true);
  write_operand(cpy_instr[1], false);

  free(instr);
  free(cpy);
  free(cpy_instr);
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

int main() {
  char buf[256];
  while (fgets (buf, sizeof(buf), stdin)) {
    if (buf[0] != '\n') {
      assemble(buf);
    }
  }
  dump_mem();
  return 1;
}
