#include "mem.h"
#include <stdlib.h>
#include "vm.h"

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

void freeObjects() {
  Obj* object = vm.objects;
  while (object != NULL) {
    // Make sure we have a reference to next before we free it
    Obj* next = object->next;

    switch (object->type) {
      case OBJ_STRING: {
        String* string = (String*) object;
        // Free the chars, then the object itself
        FREE_ARRAY(char, string->chars, string->length + 1);
        FREE_ARRAY(String, string, 1);
        break;
      }
    }
    
    object = next;
  }
}