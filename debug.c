#include <stdio.h>

#include "debug.h"
#include "value.h"

static int singleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstructino(const char* name, Chunk* chunk, int offset) {
  uint8_t constantIndex = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constantIndex);
  printValue(chunk->constants.values[constantIndex]);
  printf("'\n");
  return offset + 2;
}

void disassemble(Chunk* chunk, const char* label) {
  printf("== %s ==\n", label);

  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

// Returns the new offset (because some instructions might take multiple bytes)
int disassembleInstruction(Chunk* chunk, int offset) {
  printf("%04d ", offset);

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstructino("OP_CONSTANT", chunk, offset);
    case OP_RETURN:
      return singleInstruction("OP_RETURN", offset);
    default:
      printf("Unknown opcode: %d\n", instruction);
      return offset + 1;
  }
}
