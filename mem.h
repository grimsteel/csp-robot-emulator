#pragma once

#include "common.h"

// Double the capacity (or get it to 8 if it's less than 8)
#define GROW_CAPACITY(cap) ((cap) < 8 ? 8 : (cap) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
  (type*) reallocate((pointer), sizeof(type) * (oldCount), sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, count) \
  reallocate(pointer, sizeof(type) * (count), 0)

// Reallocate a block of memory. If oldCap is 0, just alloc
// If newCap is 0, dealloc
void* reallocate(void* pointer, size_t oldCap, size_t newCap);