#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "chunk.h"
#include "strobj.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

#include <stdio.h>
#include <stdlib.h>

// ===== TYPES AND VARIABLES =====

typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_STORE,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,
  PREC_RELATIONAL,
  PREC_ADD,
  PREC_MUL,
  PREC_UNARY,
  PREC_CALL,
  PREC_PRIMARY
} Precedence;

typedef struct {
  void (*prefix)();
  void (*infix)();
  Precedence precedence;
} ParseRule;

Parser parser;
// Current chunk that we are compiling
Chunk* compilingChunk;

static Chunk* currentChunk() {
  return compilingChunk;
}

// ===== ERRORS =====

/// Print out an error at a given token
static void errorAt(Token* token, const char* message) {
  // Don't print out more errors if we already did
  if (parser.panicMode) return;
  parser.panicMode = true;
  
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, "at end");
  } else if (token->type == TOKEN_ERROR) {
    // error tokens have no length
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

/// Error at just consumed token
static void error(const char* message) {
  errorAt(&parser.previous, message);
}

/// Error at current token
static void errorAtCurrent(const char* message) {
  errorAt(&parser.current, message);
}

// ===== PARSER UTILITIES =====

/// Advance one token
static void advance() {
  parser.previous = parser.current;

  while (true) {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR) break;

    errorAtCurrent(parser.current.start);
  }
}

/// Advance one token if it is equal to the given token. Otherwise, print error message
static void expect(TokenType type, const char* message) {
  if (parser.current.type == type) {
    advance();
  } else {
    errorAtCurrent(message);
  }
}

static uint8_t makeConstant(Value value) {
  int constant = addConstant(currentChunk(), value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk");
    return 0;
  }

  return (uint8_t) constant;
}

static void emitByte(uint8_t byte) {
  appendChunk(currentChunk(), byte, parser.previous.line);
}

static void endCompiler() {
  emitByte(OP_RETURN);
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disassemble(currentChunk(), "code");
  }
#endif
}

static ParseRule* getRule(TokenType token);
static void parsePrecedence(Precedence precedence);

// ===== INDIVIDUAL ITEM PARSING =====

static void number() {
  double value = strtod(parser.previous.start, NULL);
  emitByte(OP_CONSTANT);
  emitByte(makeConstant(NUMBER_VAL(value)));
}

static void string() {
  emitByte(OP_CONSTANT);
  // Strip off the starting/ending quotes
  emitByte(makeConstant(STRING_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2))));
}

static void boolean() {
  // Booleans are ops rather than vals at this stage for efficiency
  emitByte(parser.previous.type == TOKEN_TRUE ? OP_TRUE : OP_FALSE);
}

static void expression() {
  parsePrecedence(PREC_STORE);
}

static void unary() {
  TokenType operatorType = parser.previous.type;

  // Parse an expression
  parsePrecedence(PREC_UNARY);

  switch (operatorType) {
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    case TOKEN_NOT: emitByte(OP_NOT); break;
    default: return;
  }
}

static void binary() {
  TokenType operatorType = parser.previous.type;
  // +1 because left associativity
  Precedence prec = getRule(operatorType)->precedence + 1;
  // Parse and emit the right operator
  parsePrecedence(prec);

  // Emit the op
  switch (operatorType) {
    case TOKEN_PLUS: emitByte(OP_ADD); break;
    case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
    case TOKEN_MOD: emitByte(OP_MOD); break;
    
    case TOKEN_AND: emitByte(OP_AND); break;
    case TOKEN_OR: emitByte(OP_OR); break;
    
    case TOKEN_EQUAL: emitByte(OP_EQUAL); break;
    case TOKEN_BANG_EQUAL:
      emitByte(OP_EQUAL);
      emitByte(OP_NOT);
      break;
    case TOKEN_LESS: emitByte(OP_LESS); break;
    case TOKEN_GREATER: emitByte(OP_GREATER); break;
    case TOKEN_LESS_EQUAL:
      emitByte(OP_GREATER);
      emitByte(OP_NOT);
      break;
    case TOKEN_GREATER_EQUAL:
      emitByte(OP_LESS);
      emitByte(OP_NOT);
      break;
    default: return;
  }
}

static void parens() {
  expression();
  expect(TOKEN_RPAREN, "Expect ')' after expression.");
}

// ===== PARSE TABLE =====

ParseRule rules[] = {
  [TOKEN_LPAREN] = {parens, NULL, PREC_NONE},
  
  [TOKEN_PLUS] = {NULL, binary, PREC_ADD},
  [TOKEN_MINUS] = {unary, binary, PREC_ADD},
  [TOKEN_STAR] = {NULL, binary, PREC_MUL},
  [TOKEN_SLASH] = {NULL, binary, PREC_MUL},
  [TOKEN_MOD] = {NULL, binary, PREC_MUL},

  [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
  [TOKEN_EQUAL] = {NULL, binary, PREC_EQUALITY},

  [TOKEN_LESS] = {NULL, binary, PREC_RELATIONAL},
  [TOKEN_GREATER] = {NULL, binary, PREC_RELATIONAL},
  [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_RELATIONAL},
  [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_RELATIONAL},
  [TOKEN_LESS_DASH] = {NULL, binary, PREC_RELATIONAL},

  [TOKEN_IDENT] = {NULL, NULL, PREC_NONE},
  [TOKEN_STRING] = {string, NULL, PREC_NONE},
  // Prefix because this is parsed in the prefix stage
  [TOKEN_NUMBER] = {number, NULL, PREC_NONE},

  [TOKEN_AND] = {NULL, binary, PREC_AND},
  [TOKEN_OR] = {NULL, binary, PREC_OR},
  [TOKEN_NOT] = {unary, NULL, PREC_NONE},

  [TOKEN_TRUE] = {boolean, NULL, PREC_NONE},
  [TOKEN_FALSE] = {boolean, NULL, PREC_NONE}
};

static void parsePrecedence(Precedence precedence) {
  // Now previous will be the atom/prefix op
  // current will be the infix op (or atom, but it will be the infix op after we call the prefixRule fn)
  advance();

  // Get the rule (should be a prefix op or a primary)
  void (*prefixRule)() = getRule(parser.previous.type)->prefix;

  // error
  if (prefixRule == NULL) {
    error("Expect prefix operator or primary");
    return;
  }

  prefixRule();

  // Now parse all items that are joined by greater precedences
  while (getRule(parser.current.type)->precedence >= precedence) {
    // Previous is the infix op
    // Current is the atom/prefix op
    advance();

    void (*infixRule)() = getRule(parser.previous.type)->infix;
    infixRule();
  }
}

static ParseRule* getRule(TokenType type) {
  return &rules[type];
}

// ===== ENTRYPOINT =====

bool compile(const char* source, Chunk* chunk) {
  initScanner(source);
  compilingChunk = chunk;

  parser.panicMode = false;
  parser.hadError = false;
  
  advance();
  expression();
  expect(TOKEN_EOF, "Expect end of expression.");
  endCompiler();
  
  return !parser.hadError;
}