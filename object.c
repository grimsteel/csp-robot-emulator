#include <stdio.h>
#include <string.h>

#include "object.h"
#include "value.h"
#include "common.h"
#include "mem.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) (type*) allocateObj(sizeof(type), objectType)

Obj* allocateObj(size_t size, ObjType type) {
  Obj* object = (Obj*) reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

String* allocateString(char* chars, int length) {
  String* string = ALLOCATE_OBJ(String, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  
  return string;
}

String* toString(Value value) {
  switch (value.type) {
    case VAL_NUMBER: {
      int len = snprintf(NULL, 0, "%g", value.as.number);
      char* heapBytes = ALLOCATE(char, len + 1);
      snprintf(heapBytes, len + 1, "%g", value.as.number);
      return allocateString(heapBytes, len);
    }
    case VAL_BOOLEAN: {
      if (value.as.boolean == true) {
        char* heapBytes = ALLOCATE(char, 5);
        strcpy(heapBytes, "TRUE");
        return allocateString(heapBytes, 4);
      } else {
        char* heapBytes = ALLOCATE(char, 6);
        strcpy(heapBytes, "FALSE");
        return allocateString(heapBytes, 5);
      }
    }
    // Probably should copy the string
    case VAL_OBJECT: {
      switch (value.as.object->type) {
        case OBJ_STRING:
          return (String*) value.as.object;
      }
    }
  }
}

String* concatStrings(String* a, String* b) {
  int length = a->length + b->length;
  // alloc
  char* heapBytes = ALLOCATE(char, length + 1);
  // copy a to 0
  memcpy(heapBytes, a->chars, a->length);
  // copy b to after a ends
  memcpy(heapBytes + a->length, b->chars, b->length);
  heapBytes[length] = '\0';

  return allocateString(heapBytes, length);
}

/// Print an object depending on its type
void printObject(Obj* object) {
  switch (object->type) {
    case OBJ_STRING: {
      String* objectString = (String*) object;
      printf("\"%.*s\"", objectString->length, objectString->chars);
      break;
    }
  }
}

bool objIsFalsy(Obj* object) {
  switch (object->type) {
    case OBJ_STRING:
      return ((String*) object)->length == 0;
  }
}
bool objectsEqual(Obj* a, Obj* b) {
  // If they don't have the same type, they're not equal
  if (a->type != b->type) return false;

  switch (a->type) {
    case OBJ_STRING: {
      String* aString = (String*) a;
      String* bString = (String*) b;
      // Make sure the lengths are the same        and then do a memcmp to check the contents
      return aString->length == bString->length && memcmp(aString->chars, bString->chars, aString->length) == 0;
    }
  }
}