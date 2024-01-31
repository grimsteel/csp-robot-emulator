#pragma once

#include "common.h"
#include "value.h"

typedef enum {
  OP_RETURN,
  OP_CONSTANT, // one operand
} OpCode;

// Dynamic array of bytes
typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void appendChunk(Chunk* chunk, uint8_t byte);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value constant);