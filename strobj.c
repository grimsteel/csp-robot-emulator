#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strobj.h"
#include "value.h"
#include "common.h"

static String* allocateString(char* chars, int length) {
  String* string = (String*) calloc(1, sizeof(String));
  string->length = length;
  string->chars = chars;
  return string;
}

String* copyString(const char* chars, int length) {
  // Points right after the end
  const char* end = chars + length * sizeof(char);
  
  // The max we'll need is length + 1
  char* heapBytes = (char*) calloc(length + 1, sizeof(char));
  // Iterate through and look for escapes
  const char* currentIn = chars;
  char* currentOut = heapBytes;
  
  for (; currentIn < end; currentIn++, currentOut++) {
    if (*currentIn == '\\') {
      // See what escape char it is
      switch (*++currentIn) {
        case 'n': *currentOut = '\n'; break;
        case 'r': *currentOut = '\r'; break;
        case 'b': *currentOut = '\b'; break;
        default: *currentOut = *currentIn;
      }
    } else {
      // Normal char
      *currentOut = *currentIn;
    }
  }

  int resultantLength = currentOut - heapBytes;
  heapBytes = (char*) realloc(heapBytes, (resultantLength + 1) * sizeof(char));
  heapBytes[resultantLength] = '\0';
  return allocateString(heapBytes, resultantLength);
}