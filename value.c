#include "value.h"
#include "object.h"
#include "mem.h"

#include <stdio.h>
#include <string.h>

void initValueArray(ValueArray* valueArray) {
  valueArray->count = 0;
  valueArray->capacity = 0;
  valueArray->values = NULL;
}
void appendValueArray(ValueArray* valueArray, Value value) {
  // If we don't have space
  if (valueArray->capacity < valueArray->count + 1) {
    // Alloc more space

    int newCapacity = GROW_CAPACITY(valueArray->capacity);
    valueArray->values = GROW_ARRAY(Value, valueArray->values, valueArray->capacity, newCapacity);
    valueArray->capacity = newCapacity;
  }

  valueArray->values[valueArray->count++] = value;
}
void freeValueArray(ValueArray* valueArray) {
  FREE_ARRAY(Value, valueArray->values, valueArray->capacity);
  initValueArray(valueArray);
}

void printValue(Value value) {
  switch (value.type) {
    case VAL_BOOLEAN:
      printf(value.as.boolean ? "TRUE" : "FALSE");
      break;
    case VAL_NUMBER:
      printf("%g", value.as.number);
      break;
    case VAL_OBJECT:
      printObject(value.as.object);
      break;
  }
}

bool isFalsy(Value value) {
  switch (value.type) {
    case VAL_NUMBER:
      return value.as.number == 0.0;
    case VAL_BOOLEAN:
      return value.as.boolean == false;
    case VAL_OBJECT:
      return objIsFalsy(value.as.object);
  }
}

bool valuesEqual(Value a, Value b) {
  if (a.type != b.type) return false;

  switch (a.type) {
    case VAL_NUMBER:
      return a.as.number == b.as.number;
    case VAL_BOOLEAN:
      return a.as.boolean == b.as.boolean;
    case VAL_OBJECT:
      return objectsEqual(a.as.object, b.as.object);
  }
}