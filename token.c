#include "token.h"

const char *const datalessTokenType[] =
{
  "!=", // NOT_EQUAL_TO_OPERATOR
  "(", // LEFT_PARENTHESIS
  ")", // RIGHT_PARENTHESIS
  "*", // MULTIPLICATION_OPERATOR
  "+", // ADDITION_OPERATOR
  ",", // COMMA_OPERATOR
  "-", // SUBTRACTION_OPERATOR
  "/", // DIVISION_OPERATOR
  ";", // SEMICOLON
  "<", // LESS_THAN_OPERATOR
  "<=", // LESS_THAN_OR_EQUAL_TO_OPERATOR
  "=", // ASSIGNMENT_OPERATOR
  "==", // EQUAL_TO_OPERATOR
  ">", // GREATER_THAN_OPERATOR
  ">=", // GREATER_THAN_OR_EQUAL_TO_OPERATOR
  "{", // LEFT_CURLY_BRACKET
  "}", // RIGHT_CURLY_BRACKET
  "boolean", // BOOLEAN_DATA_TYPE
  "break", // BREAK_KEYWORD
  "class", // CLASS_KEYWORD
  "continue", // CONTINUE_KEYWORD
  "do", // DO_KEYWORD
  "double", // DOUBLE_DATA_TYPE
  "else", // ELSE_KEYWORD
  "false", // FALSE_KEYWORD
  "for", // FOR_KEYWORD
  "if", // IF_KEYWORD
  "int", // INT_DATA_TYPE
  "return", // RETURN_KEYWORD
  "String", // STRING_DATA_TYPE
  "static", // STATIC_KEYWORD
  "true", // TRUE_KEYWORD
  "void", // VOID_KEYWORD
  "while", // WHILE_KEYWORD
  ""
};

void disposeToken(Token **t)
{
  free((*t)->data);
  free(*t);
  *t = NULL;
  return;
}

Token *getToken()
{
  resetBufferPosition(mainBuffer);
  currentState = &start;
  endOfTokenFlag = 0;

  if (transferedCharacter != -1)
  {
    currentCharacter = transferedCharacter;
    currentState();
    transferedCharacter = -1;
  }

  while (1)
  {
    while (mainBuffer->position < mainBuffer->size * mainBuffer->multiplier - 1)
    {
      if ((currentCharacter = getchar()) != EOF)
      {
        currentState();
        if (endOfTokenFlag)
          return currentToken;
      }
      else if (mainBuffer->position == 0)
      {
        endOfFile();
        endLexicalAnalysis();
        return currentToken;
      }
      else
        printErrorMessageAndExit("unexpected end-of-file", 1);
    }
    extendBufferArray(mainBuffer);
  }
}

Token *initializeToken(TokenType type, char *data)
{
  Token *t = (Token *)malloc(sizeof(Token));
  if (t == NULL)
    printErrorMessageAndExit("malloc failure", 1);
  t->type = type;
  t->data = data;
  return t;
}
