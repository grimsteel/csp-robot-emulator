#pragma once

#include "common.h"

typedef double Value;

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