#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "error.h"

typedef enum
{
  NOT_EQUAL_TO_OPERATOR, // "!="
  LEFT_PARENTHESIS, // "("
  RIGHT_PARENTHESIS, // ")"
  MULTIPLICATION_OPERATOR, // "*"
  ADDITION_OPERATOR, // "+"
  COMMA_OPERATOR, // ","
  SUBTRACTION_OPERATOR, // "-"
  DIVISION_OPERATOR, // "/"
  SEMICOLON, // ";"
  LESS_THAN_OPERATOR, // "<"
  LESS_THAN_OR_EQUAL_TO_OPERATOR, // "<="
  ASSIGNMENT_OPERATOR, // "="
  EQUAL_TO_OPERATOR, // "=="
  GREATER_THAN_OPERATOR, // ">"
  GREATER_THAN_OR_EQUAL_TO_OPERATOR, // ">="
  LEFT_CURLY_BRACKET, // "{"
  RIGHT_CURLY_BRACKET, // "}"
  BOOLEAN_DATA_TYPE, // "boolean"
  BREAK_KEYWORD, // "break"
  CLASS_KEYWORD, // "class"
  CONTINUE_KEYWORD, // "continue"
  DO_KEYWORD, // "do"
  DOUBLE_DATA_TYPE, // "double"
  ELSE_KEYWORD, // "else"
  FALSE_KEYWORD, // "false"
  FOR_KEYWORD, // "for"
  IF_KEYWORD, // "if"
  INT_DATA_TYPE, // "int"
  RETURN_KEYWORD, // "return"
  STRING_DATA_TYPE, // "String"
  STATIC_KEYWORD, // "static"
  TRUE_KEYWORD, // "true"
  VOID_KEYWORD, // "void"
  WHILE_KEYWORD, // "while"
  IDENTIFIER, // "^[a-zA-Z_$][a-zA-Z0-9_$]*([.][a-zA-Z_$][a-zA-Z0-9_$]*)?$"
  INT_LITERAL, // "^[0]*[1-9][0-9]*$"
  DOUBLE_LITERAL, // "^[0-9]+[.][0-9]+([eE][+-]?[0-9]+)?$"
  STRING_LITERAL, // "^\".*\"$"
  END_OF_FILE // "-1"
} TokenType;

typedef struct
{
  TokenType type;
  char *data;
} Token;

extern Buffer *mainBuffer;
extern Token *currentToken;
extern void (*currentState)();
extern int transferedCharacter;
extern int endOfTokenFlag;
extern void start();
extern void endLexicalAnalysis();
extern void endOfFile();

void disposeToken(Token **);
Token *getToken();
Token *initializeToken(TokenType, char *);

#endif
