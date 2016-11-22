#include "error.h"

void printErrorMessageAndExit(char *errorMessage, int returnValue)
{
  fprintf(stderr, "ERROR: %s!\n", errorMessage);
  exit(returnValue);
}
