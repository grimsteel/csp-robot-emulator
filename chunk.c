#include "chunk.h"
#include "mem.h"
#include <stdlib.h>

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initValueArray(&chunk->constants);
}

// Append a byte to a chunk
void appendChunk(Chunk* chunk, uint8_t byte) {
  // If we don't have space
  if (chunk->capacity < chunk->count + 1) {
    // Alloc more space

    int newCapacity = GROW_CAPACITY(chunk->capacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, chunk->capacity, newCapacity);
    chunk->capacity = newCapacity;
  }

  chunk->code[chunk->count++] = byte;
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

int addConstant(Chunk* chunk, Value constant) {
  appendValueArray(&chunk->constants, constant);
  // Return the index of the added value
  return chunk->constants.count - 1;
}