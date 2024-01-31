#pragma once

#include "chunk.h"

void disassemble(Chunk* chunk, const char* label);
int disassembleInstruction(Chunk* chunk, int offset);