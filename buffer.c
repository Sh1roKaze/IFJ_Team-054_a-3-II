#include "buffer.h"

void addCharacterToBuffer(Buffer *b, char c)
{
  b->array[b->position] = c;
  b->position++;
  return;
}

void addCurrentCharacterToBuffer(Buffer *b)
{
  addCharacterToBuffer(b, currentCharacter);
  return;
}

char *copyBufferArray(Buffer *b)
{
  addCharacterToBuffer(b, '\0');
  char *string = (char *)malloc(b->position);
  strcpy(string, b->array);
  return string;
}

void disposeBuffer(Buffer **b)
{
  free((*b)->array);
  free(*b);
  *b = NULL;
  return;
}

void extendBufferArray(Buffer *b)
{
  b->multiplier *= 2;
  b->array = (char *)realloc(b->array, b->size * b->multiplier);
  if (b->array != NULL)
    return;
  printErrorMessageAndExit("realloc failure", 1);
}

Buffer *initializeBuffer(unsigned int size)
{
  Buffer *b = (Buffer *)malloc(sizeof(Buffer));
  if (b != NULL)
  {
    b->size = size;
    b->multiplier = 1;
    b->position = 0;
    b->array = (char *)malloc(b->size * b->multiplier);
    if (b->array != NULL)
      return b;
    free(b);
  }
  printErrorMessageAndExit("malloc failure", 1);
  return NULL;
}

int isBufferEmpty(Buffer *b)
{
  return (b->position == 0);
}

void resetBufferPosition(Buffer *b)
{
  b->position = 0;
  return;
}
