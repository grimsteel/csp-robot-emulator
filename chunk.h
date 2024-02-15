#pragma once

#include "common.h"
#include "value.h"

typedef enum {
  OP_RETURN,
  OP_CONSTANT, // one operand
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_AND,
  OP_OR,
  OP_NOT,
  OP_TRUE,
  OP_FALSE,
  OP_MOD
} OpCode;

// Dynamic array of bytes
typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  int* lines;
  ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void appendChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value constant);