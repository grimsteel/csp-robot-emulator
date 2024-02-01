#include "common.h"
#include "chunk.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
  initVM();

  Chunk chunk;
  
  initChunk(&chunk);

  int constantIndex = addConstant(&chunk, 4.2);

  appendChunk(&chunk, OP_CONSTANT, 1);
  appendChunk(&chunk, addConstant(&chunk, 1.2), 1);

  appendChunk(&chunk, OP_CONSTANT, 1);
  appendChunk(&chunk, addConstant(&chunk, 3.4), 1);

  appendChunk(&chunk, OP_ADD, 1);

  appendChunk(&chunk, OP_CONSTANT, 2);
  appendChunk(&chunk, addConstant(&chunk, 5.6), 2);

  appendChunk(&chunk, OP_DIVIDE, 2);
  appendChunk(&chunk, OP_NEGATE, 2);
  appendChunk(&chunk, OP_RETURN, 3);

  interpret(&chunk);
  
  freeVM();
  freeChunk(&chunk);
  
  return 0;
}