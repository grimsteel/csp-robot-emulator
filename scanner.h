#pragma once

typedef enum {
  // single char
  TOKEN_LPAREN, TOKEN_RPAREN,
  TOKEN_LBRACE, TOKEN_RBRACE,
  TOKEN_LBRACKET, TOKEN_RBRACKET,
  TOKEN_COMMA, TOKEN_EQUAL,
  TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
  TOKEN_LESS, TOKEN_GREATER,
  // multi char
  TOKEN_BANG_EQUAL, TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL,
  TOKEN_LESS_DASH, // store operator
  // literals
  TOKEN_IDENT, TOKEN_STRING, TOKEN_NUMBER,
  // keywords
  TOKEN_AND, TOKEN_OR, TOKEN_NOT,
  TOKEN_TRUE, TOKEN_FALSE,
  TOKEN_MOD,
  TOKEN_IF, TOKEN_ELSE,
  TOKEN_REPEAT, TOKEN_TIMES, TOKEN_UNTIL,
  TOKEN_FOR, TOKEN_EACH, TOKEN_IN,
  TOKEN_PROCEDURE, TOKEN_RETURN,

  TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  const char* start;
  int length;
  int line;
} Token;

void initScanner(const char* source);
Token scanToken();