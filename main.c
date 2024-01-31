#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  
  initChunk(&chunk);

  int constantIndex = addConstant(&chunk, 4.2);
  appendChunk(&chunk, OP_CONSTANT);
  appendChunk(&chunk, constantIndex);
  appendChunk(&chunk, OP_CONSTANT);
  appendChunk(&chunk, addConstant(&chunk, 1000.10));
  
  appendChunk(&chunk, OP_RETURN);

  disassemble(&chunk, "TEST_CHUNK");
  
  freeChunk(&chunk);
  
  return 0;
}