#include "lex_anal.h"

Buffer *mainBuffer; // is used to store characters which have already been read
Token *currentToken; // will be initialized when the final state is reached
State (*currentState)(); // points to the function corrensponding to the current state
int transferedCharacter; // will be used to store next character when the final state is reached
int currentCharacter; // holds last read character
int currentDatalessTokenType; // holds number of current dataless token or equals to -1 otherwise
int endOfTokenFlag; // indicates, if the final state was reached (positive integer) or not (0)

void beginLexicalAnalysis()
{
  mainBuffer = initializeBuffer(BUFFER_SIZE); // BUFFER_SIZE = 1024
  return;
}

State datalessType()
{
  //printf("[currentState] DATALESS TYPE: %c | %x | %d | %c | %x | %lx\n", currentCharacter, currentCharacter, mainBuffer->position, datalessTokenType[currentDatalessTokenType][mainBuffer->position], datalessTokenType[currentDatalessTokenType][mainBuffer->position], strlen(datalessTokenType[currentDatalessTokenType]));

  if (currentCharacter != datalessTokenType[currentDatalessTokenType][mainBuffer->position]) // dataless type continues with unexpected character
  {
    if (mainBuffer->position == 1 && currentDatalessTokenType == DIVISION_OPERATOR) // division operator occurance could also mean beginning of comment
    {
        divisionOperatorHelpingFunction();
        return;
    }
    else if (mainBuffer->position == 1 && datalessTokenType[currentDatalessTokenType][0] == datalessTokenType[currentDatalessTokenType + 1][0] && currentCharacter == datalessTokenType[currentDatalessTokenType + 1][1]) // verifying second dataless type string with the same first character
      currentDatalessTokenType++;
    else if (currentDatalessTokenType == DO_KEYWORD && mainBuffer->position == 2 && currentCharacter == 'u')
      currentDatalessTokenType++;
    else if (currentDatalessTokenType > RIGHT_CURLY_BRACKET) // it's probably not a dataless type but identifier or operator (next token)
    {
      if (mainBuffer->position < strlen(datalessTokenType[currentDatalessTokenType]) && (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1))
      {
        transferedCharacter = currentCharacter;
        endCurrentTokenWithData(IDENTIFIER);
        return;
      }
      else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1)
      {
        transferedCharacter = currentCharacter;
        endCurrentToken(currentDatalessTokenType);
        return;
      }
      else if (currentCharacterMatchIdentifierNextLetter())
        currentState = &identifier;
      else if (currentCharacter == '.')
        currentState = &identifierDot;
      else // it's neither dataless type nor identifier (it will be analyzed in next getToken() function call
        printErrorMessageAndExit("no keyword matched", 1);
    }
    else // it's neither dataless type nor identifier (it will be analyzed in next getToken() function call
    {
      transferedCharacter = currentCharacter;
      endCurrentToken(currentDatalessTokenType);
      return;
    }
  }

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

void divisionOperatorHelpingFunction()
{
  if (currentCharacter == '/')
    currentState = &ignoreLine;
  else if (currentCharacter == '*')
    currentState = &ignoreBlock;
  else //if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1 || currentCharacterMatchRange('0', '9'))
    {
      transferedCharacter = currentCharacter;
      endCurrentToken(currentDatalessTokenType);
      return;
    }
  //else
    //printErrorMessageAndExit("ivalid keyword specification", 1);

  mainBuffer->position--;
  return;
}

State doubleLiteralDot()
{
  //printf("[currentState] DOUBLE LITERAL DOT: %c\n", currentCharacter);

  if (currentCharacterMatchRange('0', '9'))
    currentState = &doubleLiteralFraction;
  else
    printErrorMessageAndExit("invalid double format", 1);

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralExponent()
{
  //printf("[currentState] DOUBLE LITERAL EXPONENT: %c\n", currentCharacter);

  if (currentCharacter == '-' || currentCharacter == '+')
    currentState = &doubleLiteralSign;
  else if (currentCharacterMatchRange('0', '9'))
    currentState = &doubleLiteralFinish;
  else
    printErrorMessageAndExit("invalid double format", 1);

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralFinish()
{
  //printf("[currentState] DOUBLE LITERAL FINISH: %c\n", currentCharacter);

  if (!currentCharacterMatchRange('0', '9'))
  {
    if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1)
    {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(DOUBLE_LITERAL);
      return;
    }
    else
      printErrorMessageAndExit("invalid double format", 1);
  }

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralFraction()
{
  //printf("[currentState] DOUBLE LITERAL FRACTION: %c\n", currentCharacter);

  if (!currentCharacterMatchRange('0', '9'))
  {
    if (currentCharacter == 'e' || currentCharacter == 'E')
      currentState = &doubleLiteralExponent;
    else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1)
    {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(DOUBLE_LITERAL);
      return;
    }
    else
      printErrorMessageAndExit("invalid double format", 1);
  }

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralSign()
{
  //printf("[currentState] DOUBLE LITERAL SIGN: %c\n", currentCharacter);

  if (currentCharacterMatchRange('0', '9'))
    currentState = &doubleLiteralFinish;
  else
    printErrorMessageAndExit("invalid double format", 1);

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

void endCurrentToken(TokenType type)
{
  currentToken = initializeToken(type, NULL);
  endOfTokenFlag = 1;
  return;
}

void endCurrentTokenWithData(TokenType type)
{
  currentToken = initializeToken(type, copyBufferArray(mainBuffer));
  endOfTokenFlag = 1;
  return;
}

void endLexicalAnalysis()
{
  disposeBuffer(&mainBuffer);
  return;
}

State endOfFile()
{
  //printf("[currentState] END OF FILE: %c\n", currentCharacter);

  endCurrentToken(END_OF_FILE);

  return;
}

State identifier()
{
  //printf("[currentState] IDENTIFIER: %c\n", currentCharacter);

  identifierHelpingFunction(1);

  return;
}

State identifierDot()
{
  //printf("[currentState] IDENTIFIER DOT: %c\n", currentCharacter);

  if (currentCharacterMatchIdentifierFirstLetter())
    currentState = &identifierFinish;
  else
    printErrorMessageAndExit("invalid identifier format", 1);

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State identifierFinish()
{
  //printf("[currentState] IDENTIFIER FINISH: %c\n", currentCharacter);

  identifierHelpingFunction(0);

  return;
}

void identifierHelpingFunction(int acceptDotFlag)
{
  if (!currentCharacterMatchIdentifierNextLetter())
  {
    if (currentCharacter == '.' && acceptDotFlag)
      currentState = &identifierDot;
    else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1)
    {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(IDENTIFIER);
      return;
    }
    else
      printErrorMessageAndExit("invalid identifier format", 1);
  }

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State ignoreBlock()
{
  //printf("[currentState] IGNORE BLOCK: %c\n", currentCharacter);

  if (currentCharacter == '*')
    currentState = &ignoreBlockAsterisk;

  return;
}

State ignoreBlockAsterisk()
{
  //printf("[currentState] IGNORE BLOCK ASTERISK: %c\n", currentCharacter);

  if (currentCharacter == '/')
    currentState = &start;
  else
    currentState = &ignoreBlock;

  return;
}

State ignoreLine()
{
  //printf("[currentState] IGNORE LINE: %c\n", currentCharacter);

  if (currentCharacter == '\n')
    currentState = &start;

  return;
}

State numberLiteral()
{
  //printf("[currentState] NUMBER LITERAL: %c\n", currentCharacter);

  if (!currentCharacterMatchRange('0', '9'))
  {
    if (currentCharacter == '.')
      currentState = &doubleLiteralDot;
    else if (currentCharacter == 'e' || currentCharacter == 'E')
      currentState = &doubleLiteralExponent;
    else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1)
    {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(INT_LITERAL);
      return;
    }
    else
      printErrorMessageAndExit("invalid integer format", 1);
  }

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State start()
{
  //printf("[currentState] START: %x | %c\n", currentCharacter, currentCharacter);

  if (currentCharacter == currentCharacterMatchWhitespace())
    return;
  else if (currentCharacter == '\"')
  {
    currentState = &stringLiteral;
    return;
  }
  else if ((currentDatalessTokenType = currentCharacterMatchDatalessTokenType()) != -1)
    currentState = &datalessType;
  else if (currentCharacterMatchRange('0', '9'))
    currentState = &numberLiteral;
  else if (currentCharacterMatchString("_$aghjklmnopqruxyzABCDEFGHIJKLMNOPQRTUVWXYZ")) //first letters of keywords omitted
    currentState = &identifier;
  else
    printErrorMessageAndExit("invalid lexeme format", 1);

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State stringLiteral()
{
  //printf("[currentState] STRING LITERAL: %c\n", currentCharacter);

  if (currentCharacter == '\\')
  {
    currentState = &stringLiteralBackslash;
    return;
  }
  else if (currentCharacter == '\"')
  {
    endCurrentTokenWithData(STRING_LITERAL);
    return;
  }

  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State stringLiteralBackslash()
{
  //printf("[currentState] STRING LITERAL BACKSLASH: %c\n", currentCharacter);

  if (currentCharacterMatchRange('0', '3'))
  {
    transferedCharacter = currentCharacter;
    currentState = &stringLiteralOctalEscapeSequence;
    return;
  }
  else if (currentCharacter == '\"')
    addCharacterToBuffer(mainBuffer, '\"');
  else if (currentCharacter == 'n')
    addCharacterToBuffer(mainBuffer, '\n');
  else if (currentCharacter == 't')
    addCharacterToBuffer(mainBuffer, '\t');
  else if (currentCharacter == '\\')
    addCharacterToBuffer(mainBuffer, '\\');
  else
  {
    addCharacterToBuffer(mainBuffer, '\\');
    addCurrentCharacterToBuffer(mainBuffer);
  }

  currentState = &stringLiteral;
  return;
}

State stringLiteralOctalEscapeSequence()
{
  //printf("[currentState] STRING LITERAL OCTAL ESCAPE SEQUENCE: %c\n", currentCharacter);

  static char octalEscapeSequence[4] = { '\0', '\0', '\0', '\0' };
  static int characterCounter = 1;

  if (transferedCharacter != -1)
  {
    octalEscapeSequence[0] = transferedCharacter;
    transferedCharacter = -1;
  }

  if (currentCharacterMatchRange('0', '7'))
    octalEscapeSequence[characterCounter++] = currentCharacter;
  else
  {
    for (int i = 0; i < characterCounter; i++)
      addCharacterToBuffer(mainBuffer, octalEscapeSequence[i]);
    memset(octalEscapeSequence, '\0', 3);
    characterCounter = 1;

    if (currentCharacter == '\\')
      currentState = &stringLiteralBackslash;
    else if (currentCharacter == '\"')
      endCurrentTokenWithData(STRING_LITERAL);
    return;
  }

  if (characterCounter == 3)
  {
    addCharacterToBuffer(mainBuffer, strtol(octalEscapeSequence, NULL, 8));
    printf("casted: %ld\n", strtol(octalEscapeSequence, NULL, 8));
    memset(octalEscapeSequence, '\0', 3);
    characterCounter = 1;
    currentState = &stringLiteral;
  }

  return;
}
