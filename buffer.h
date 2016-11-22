#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

typedef struct
{
  char *array; // dynamic array
  unsigned int size; // original size
  unsigned int position; // current position
  unsigned int multiplier; // used to multiply size
} Buffer;

extern int currentCharacter;

void addCharacterToBuffer(Buffer *, char);
void addCurrentCharacterToBuffer(Buffer *);
char *copyBufferArray(Buffer *);
void disposeBuffer(Buffer **);
void extendBufferArray(Buffer *);
Buffer *initializeBuffer(unsigned int);
int isBufferEmpty(Buffer *);
void resetBufferPosition(Buffer *);

#endif
