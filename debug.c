#include <stdio.h>

#include "debug.h"
#include "value.h"

static int singleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
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
#define SINGLE(instruction) \
  case instruction: \
    return singleInstruction(#instruction, offset)

  printf("%04d ", offset);

  // Same line as previous
  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
    printf("   | ");
  } else {
    printf("%4d ", chunk->lines[offset]);
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    SINGLE(OP_RETURN);
    SINGLE(OP_NEGATE);
    SINGLE(OP_TRUE);
    SINGLE(OP_FALSE);
    SINGLE(OP_EQUAL);
    SINGLE(OP_LESS);
    SINGLE(OP_GREATER);
    SINGLE(OP_AND);
    SINGLE(OP_OR);
    SINGLE(OP_NOT);
    SINGLE(OP_ADD);
    SINGLE(OP_SUBTRACT);
    SINGLE(OP_MULTIPLY);
    SINGLE(OP_DIVIDE);
    SINGLE(OP_MOD);
    default:
      printf("Unknown opcode: %d\n", instruction);
      return offset + 1;
  }

#undef SINGLE
}
