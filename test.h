#ifndef TEST_H
#define TEST_H

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "lex_anal.h"

#define ARRAY_SIZE 3

char *getEnumeratorName(TokenType);
Token *newToken(TokenType, char *);
void printToken(Token *);
int stringToInteger(char *);
void testBuffer(Buffer **, char *);
void testLexicalAnalysis();
void testPointerSizes();
void testToken();

#endif
