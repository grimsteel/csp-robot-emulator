#pragma once

#include "common.h"

typedef struct Obj Obj;
typedef struct String String;

typedef enum {
  VAL_BOOLEAN,
  VAL_NUMBER,
  VAL_OBJECT
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    Obj* object;
  } as;
} Value;

// Macros for quickly creating Values
#define BOOLEAN_VAL(value) ((Value){VAL_BOOLEAN, {.boolean = value}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJECT_VAL(value) ((Value){VAL_OBJECT, {.object = value}})

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

bool isFalsy(Value value);
bool valuesEqual(Value a, Value b);