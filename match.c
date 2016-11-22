#include "match.h"

int currentCharacterMatchDatalessTokenType()
{
  int returnCode;

  if ((returnCode = currentCharacterMatchOperatorTokenType()) == -1)
    if ((returnCode = currentCharacterMatchKeywordTokenType()) == -1)
      return -1;
  return returnCode;
}

int currentCharacterMatchIdentifierFirstLetter()
{
  return (currentCharacterMatchRange('a', 'z') || currentCharacterMatchRange('A', 'Z') || currentCharacterMatchString("_$"));
}

int currentCharacterMatchIdentifierNextLetter()
{
  return (currentCharacterMatchIdentifierFirstLetter() || currentCharacterMatchRange('0', '9'));
}

int currentCharacterMatchKeywordTokenType()
{
  for (int i = BOOLEAN_DATA_TYPE; i < WHILE_KEYWORD + 1; i++)
    if (currentCharacter == datalessTokenType[i][0])
      return i;
  return -1;
}

int currentCharacterMatchOperatorTokenType()
{
  for (int i = NOT_EQUAL_TO_OPERATOR; i < BOOLEAN_DATA_TYPE; i++)
    if (currentCharacter == datalessTokenType[i][0])
      return i;
  return -1;
}

char currentCharacterMatchRange(char first, char last)
{
  for (int i = first; i < last + 1; i++)
    if (currentCharacter == i)
      return currentCharacter;
  return '\0';
}

char currentCharacterMatchString(char *string)
{
  for (int i = 0; string[i] != '\0'; i++)
    if (currentCharacter == string[i])
      return currentCharacter;
  return '\0';
}

char currentCharacterMatchWhitespace()
{
  return currentCharacterMatchString(" \n\t");
}
