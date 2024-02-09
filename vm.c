#include "common.h"
#include "vm.h"
#include "value.h"
#include "debug.h"
#include "compiler.h"
#include <stdio.h>

VM vm;

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
  resetStack();
}

void freeVM() {

}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
  do { \
    double b = popStack(); \
    double a = popStack(); \
    pushStack(a op b); \
  } while (false) // block macro trick

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    // Print the stack and the instruction
    printf("          ");
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");

    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        // Push constant to stack
        Value constant = READ_CONSTANT();
        pushStack(constant);
        break;
      }
      case OP_RETURN:
        // Print the value on the stack
        printValue(popStack());
        printf("\n");
        return INTERPRET_OK;
      case OP_NEGATE: pushStack(-popStack()); break;
      case OP_ADD: BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE: BINARY_OP(/); break;
      default:
        break;
    }
  }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  // Compile into the chunk
  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();
  
  freeChunk(&chunk);
  return result;
}

void pushStack(Value value) {
  // Post increment and dereference
  *vm.stackTop++ = value;
}

Value popStack() {
  // Pre decrement and dereference
  return *--vm.stackTop;
}