#ifndef MATCH_H
#define MATCH_H

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

extern const char *const datalessTokenType[];
extern int currentCharacter;

int currentCharacterMatchDatalessTokenType();
int currentCharacterMatchIdentifierFirstLetter();
int currentCharacterMatchIdentifierNextLetter();
int currentCharacterMatchKeywordTokenType();
int currentCharacterMatchOperatorTokenType();
char currentCharacterMatchRange(char, char);
char currentCharacterMatchString(char *);
char currentCharacterMatchWhitespace();

#endif
