#include "mem.h"
#include <stdlib.h>

void* reallocate(void* pointer, size_t oldCap, size_t newCap) {
  if (newCap == 0) {
    // Free the existing allocation
    free(pointer);
    return NULL;
  } else {
    void* result = realloc(pointer, newCap);
    if (result == NULL) exit(1);
    return result;
  }
}