#pragma once

#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj* next;
};

struct String {
  Obj obj;
  int length;
  char* chars;
};

String* allocateString(char* chars, int length);
String* toString(Value value);
String* concatStrings(String* a, String* b);

void printObject(Obj* object);
bool objIsFalsy(Obj* object);
bool objectsEqual(Obj* a, Obj* b);