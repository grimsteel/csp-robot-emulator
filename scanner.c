#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  const char* start;
  const char* current;
  int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

static bool isAtEnd() {
  return *scanner.current == '\0';
}

static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static char advance() {
  return *scanner.current++;
}

// Conditional advance
static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;
  scanner.current++;
  return true;
}

static Token errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int) strlen(message);
  token.line = scanner.line;
  return token;
}

Token scanToken() {
  // Move the token start position up
  scanner.start = scanner.current;

  if (isAtEnd()) return makeToken(TOKEN_EOF);

  char c = advance();
  switch (c) {
    case '(': return makeToken(TOKEN_LPAREN);
    case ')': return makeToken(TOKEN_RPAREN);
    case '{': return makeToken(TOKEN_LBRACE);
    case '}': return makeToken(TOKEN_RBRACE);
    case '[': return makeToken(TOKEN_LBRACKET);
    case ']': return makeToken(TOKEN_RBRACKET);
    case ',': return makeToken(TOKEN_COMMA);
    case '=': return makeToken(TOKEN_EQUAL);
    case '+': return makeToken(TOKEN_PLUS);
    case '-': return makeToken(TOKEN_MINUS);
    case '*': return makeToken(TOKEN_STAR);
    case '/': return makeToken(TOKEN_SLASH);
    case '<': return makeToken(TOKEN_LESS);
    case '>': return makeToken(TOKEN_GREATER);
  }

  return errorToken("Unexpected character.");
}