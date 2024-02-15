#include "common.h"
#include "vm.h"
#include "value.h"
#include "debug.h"
#include "compiler.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

VM vm;

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
  resetStack();
}

void freeVM() {

}

/// Return value without popping
static Value peek(int distance) {
  return vm.stackTop[-1 - distance];
}

static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
// Binary op between **two numbers**. valueType defines output
// (Will be number for arithmetic ops, bool for comparative ops)
#define BINARY_OP(valueType, op) \
  do { \
    if (peek(0).type != VAL_NUMBER || peek(1).type != VAL_NUMBER) { \
      runtimeError("Both operands must be numbers."); \
      return INTERPRET_RUNTIME_ERR; \
    } \
    double b = popStack().as.number; \
    double a = popStack().as.number; \
    pushStack(valueType(a op b)); \
  } while (false)

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
      case OP_NEGATE:
        // needs to be a number
        if (peek(0).type != VAL_NUMBER) {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERR;
        }
        pushStack(NUMBER_VAL(-popStack().as.number));
        break;
      case OP_TRUE: pushStack(BOOLEAN_VAL(true)); break;
      case OP_FALSE: pushStack(BOOLEAN_VAL(false)); break;
      case OP_NOT: pushStack(BOOLEAN_VAL(isFalsy(popStack()))); break;
      case OP_LESS: BINARY_OP(BOOLEAN_VAL, <); break;
      case OP_GREATER: BINARY_OP(BOOLEAN_VAL, >); break;
      case OP_AND: {
        // A AND B == !(!A OR !B)
        bool b = isFalsy(popStack());
        bool a = isFalsy(popStack());
        pushStack(BOOLEAN_VAL(!(a || b)));
        break;
      }
      case OP_OR: {
        // A OR B == !(!A AND !B)
        bool b = isFalsy(popStack());
        bool a = isFalsy(popStack());
        pushStack(BOOLEAN_VAL(!(a && b)));
        break;
      }
      case OP_EQUAL: {
        Value b = popStack();
        Value a = popStack();
        pushStack(BOOLEAN_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_ADD: BINARY_OP(NUMBER_VAL, +); break;
      case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
      case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
      case OP_DIVIDE: BINARY_OP(NUMBER_VAL, /); break;
      case OP_MOD: {
        // Mod is different bc we can't use built in '%' operator
        if (peek(0).type != VAL_NUMBER || peek(1).type != VAL_NUMBER) {
          runtimeError("Both operands must be numbers.");
          return INTERPRET_RUNTIME_ERR;
        }
        double b = popStack().as.number;
        double a = popStack().as.number;
        pushStack(NUMBER_VAL(fmod(a, b)));
        break;
      }
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