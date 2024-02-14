#pragma once

#include "common.h"

typedef enum {
  VAL_BOOLEAN,
  VAL_NUMBER,
  VAL_STRING
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    char* string;
  } as;
} Value;

// Macros for quickly creating Values
#define BOOLEAN_VAL(value) ((Value){VAL_BOOLEAN, {.boolean = value}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define STRING_VAL(value) ((Value){VAL_STRING, {.string = value}})

// Dynamic array of values
typedef struct {
  int count;
  int capacity;
  Value* values;
} ValueArray;

void initValueArray(ValueArray* valueArray);
void appendValueArray(ValueArray* valueArray, Value value);
void freeValueArray(ValueArray* valueArray);

void printValue(Value value);