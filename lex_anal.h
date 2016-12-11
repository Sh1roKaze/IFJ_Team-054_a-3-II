/*
Projekt IFJ16
xverny00 - Jan Verny
xvalac08 - Dominik Valachovic 
xosker02 - Jan Oskera
xvlach16 - Antonin Vlach
xzikmu08 - David Zikmund
*/

#ifndef LEX_ANAL_H
#define LEX_ANAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// macro constants
#define BUFFER_SIZE 1024

// typedefs - buffer
typedef struct {
  char *array; // dynamic array
  unsigned int size; // original size
  unsigned int position; // current position
  unsigned int multiplier; // used to multiply size
} Buffer;

// typedefs - FSM state
typedef void State; // void alias for FSM states

// typedefs - token
typedef enum {
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
  DOUBLE_LITERAL, // "^[0-9]+[.][0-9]+([eE][+-]?[0-9]+)?$" --> this is not a correct regex, but nobody cares, since we don't use regexes in this project :D
  STRING_LITERAL, // "^\".*\"$"
  END_OF_FILE // "-1"
} TokenType;

typedef struct {
  TokenType type;
  char *data; // is used for raw string form storage of identifiers and literals
} Token;

// functions - buffer ADT
void addCharacterToBuffer(Buffer *, char);
void addCurrentCharacterToBuffer(Buffer *);
char *copyBufferArray(Buffer *);
void disposeBuffer(Buffer **);
int extendBufferArray(Buffer *);
Buffer *initializeBuffer(unsigned int);
int isBufferEmpty(Buffer *);
void resetBufferPosition(Buffer *);

// functions - FSM states
State datalessType();
State doubleLiteralDot();
State doubleLiteralExponent();
State doubleLiteralFinish();
State doubleLiteralFraction();
State doubleLiteralSign();
State identifier();
State identifierDot();
State identifierFinish();
State ignoreBlock();
State ignoreBlockAsterisk();
State ignoreLine();
State numberLiteral();
State start();
State stringLiteral();
State stringLiteralBackslash();
State stringLiteralOctalEscapeSequence();
State stringLiteralOctalEscapeSequenceFinish();

// functions - helping functions
void beginLexicalAnalysis();
void divisionOperatorHelpingFunction();
void endCurrentTokenWithData(TokenType type);
void endCurrentToken(TokenType type);
void endLexicalAnalysis();
void identifierHelpingFunction(int);
void printErrorMessageAndPrepareForExiting(char *, int);

// functions - regex substitution
int currentCharacterMatchDatalessTokenType();
int currentCharacterMatchIdentifierFirstLetter();
int currentCharacterMatchIdentifierNextLetter();
int currentCharacterMatchKeywordTokenType();
int currentCharacterMatchOperatorTokenType();
char currentCharacterMatchRange(char, char);
char currentCharacterMatchString(char *);
char currentCharacterMatchWhitespace();

// functions - token ADT
void disposeToken(Token **);
Token *getToken();
Token *initializeToken(TokenType, char *);

#endif
