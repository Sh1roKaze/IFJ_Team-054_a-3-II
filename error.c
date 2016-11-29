#include "error.h"

void printErrorMessageAndExit(char *errorMessage, int returnValue)
{
  endLexicalAnalysis(); // freeing of buffer

  fprintf(stderr, "ERROR: %s!\n", errorMessage);
  exit(returnValue);
}
