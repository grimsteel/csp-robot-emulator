#include "value.h"
#include "mem.h"

#include <stdio.h>

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
  printf("%g", value);
}