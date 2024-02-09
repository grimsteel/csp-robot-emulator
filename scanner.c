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
  // Return the dereferenced current token and then advance
  return *scanner.current++;
}

static char peek() {
  return *scanner.current;
}

// Conditional advance
static bool match(char expected) {
  if (isAtEnd()) return false;
  // If the current token is not the expected, don't advance past it
  if (peek() != expected) return false;
  advance();
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

void skipWhitespace() {
  while (true) {
    switch (peek()) {
      case '\n':
        scanner.line++;
      // fall through
      case ' ':
      case '\t':
      case '\r':
        advance();
        break;
      default:
        // Stop at a non whitespace character
        return;
    }
  }
}

static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
  if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
    return type;
  }

  return TOKEN_IDENT;
}

static TokenType getIdentType() {
  int length = scanner.current - scanner.start;
  
  switch (*scanner.start) {
    case 'A': return checkKeyword(1, 2, "ND", TOKEN_AND);
    case 'E':
      if (length > 1) {
        if (scanner.start[1] == 'L') return checkKeyword(2, 2, "SE", TOKEN_ELSE);
        else if (scanner.start[1] == 'A') return checkKeyword(2, 2, "CH", TOKEN_EACH);
      }
      break;
    case 'F':
      if (length > 1) {
        if (scanner.start[1] == 'O') return checkKeyword(2, 1, "R", TOKEN_FOR);
        else if (scanner.start[1] == 'A') return checkKeyword(2, 3, "LSE", TOKEN_FALSE);
      }
      break;
    case 'I':
      if (length == 2) {
        if (scanner.start[1] == 'F') return TOKEN_IF;
        else if (scanner.start[1] == 'N') return TOKEN_IN;
      }
      break;
    case 'M': return checkKeyword(1, 2, "OD", TOKEN_MOD);
    case 'N': return checkKeyword(1, 2, "OT", TOKEN_NOT);
    case 'O': return checkKeyword(1, 1, "R", TOKEN_OR);
    case 'P': return checkKeyword(1, 8, "ROCEDURE", TOKEN_PROCEDURE);
    case 'R':
      if (length > 2 && scanner.start[1] == 'E') {
        if (scanner.start[2] == 'T') return checkKeyword(3, 3, "URN", TOKEN_RETURN);
        else if (scanner.start[2] == 'P') return checkKeyword(3, 3, "EAT", TOKEN_REPEAT);
      }
      break;
    case 'T':
      if (length > 1) {
        if (scanner.start[1] == 'I') return checkKeyword(2, 3, "MES", TOKEN_TIMES);
        else if (scanner.start[1] == 'R') return checkKeyword(2, 2, "UE", TOKEN_TRUE);
      }
      break;
    case 'U': return checkKeyword(1, 4, "NTIL", TOKEN_UNTIL);
  }
  return TOKEN_IDENT;
}

Token scanToken() {
  // First go past all whitespace
  skipWhitespace();
  // Move the token start position up
  scanner.start = scanner.current;

  if (isAtEnd()) return makeToken(TOKEN_EOF);

  char c = advance();
  
  // Numbers
  if (isDigit(c)) {
    while (isDigit(peek())) advance();

    if (match('.')) {
      while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
  }
  // Idents
  if (isAlpha(c)) {
    while (isDigit(peek()) || isAlpha(peek())) advance();
    return makeToken(getIdentType());
  }
  
  switch (c) {
    case '(': return makeToken(TOKEN_LPAREN);
    case ')': return makeToken(TOKEN_RPAREN);
    case '{': return makeToken(TOKEN_LBRACE);
    case '}': return makeToken(TOKEN_RBRACE);
    case '[': return makeToken(TOKEN_LBRACKET);
    case ']': return makeToken(TOKEN_RBRACKET);
    case ',': return makeToken(TOKEN_COMMA);
    case '=': return makeToken(TOKEN_EQUAL);
    case '!': 
      if (match('=')) return makeToken(TOKEN_BANG_EQUAL);
      else break; // '!' cannot stand by itself (CSP Pseudocode uses "NOT")
    case '+': return makeToken(TOKEN_PLUS);
    case '-': return makeToken(TOKEN_MINUS);
    case '*': return makeToken(TOKEN_STAR);
    case '/': return makeToken(TOKEN_SLASH);
    case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : match('-') ? TOKEN_LESS_DASH : TOKEN_LESS);
    case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    // Literals
    case '"':
      // Parse a full string
      while (true) {
        if (isAtEnd() || peek() == '\n') return errorToken("Unterminated string.");
        // Skip escapes for now. Skip over the \ and the escape char
        if (match('\\')) advance();
        if (match('"')) break;

        advance();
      }
      return makeToken(TOKEN_STRING);
  }

  return errorToken("Unexpected character.");
}