#pragma once

#include "value.h"

struct String {
  int length;
  char* chars;
};

String* copyString(const char* chars, int length);