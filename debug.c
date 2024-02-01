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
    case OP_RETURN:
      return singleInstruction("OP_RETURN", offset);
    case OP_NEGATE:
      return singleInstruction("OP_NEGATE", offset);
    case OP_ADD:
      return singleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
      return singleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
      return singleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
      return singleInstruction("OP_DIVIDE", offset);
    default:
      printf("Unknown opcode: %d\n", instruction);
      return offset + 1;
  }
}
