#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
  KEYWORD, // "break", "class", "continue", "do", "else", "false", "for", "if", "return", "static", "true", "void", "while"
  IDENTIFIER, //
  BOOLEAN_DATA_TYPE, // boolean
  INT_DATA_TYPE, // int
  DOUBLE_DATA_TYPE, // double
  STRING_DATA_TYPE, // String
  INT_LITERAL, //
  DOUBLE_LITERAL, //
  STRING_LITERAL, //
  ASSIGNMENT_OPERATOR, // "="
  ADDITION_OPERATOR, // "+"
  SUBTRACTION_OPERATOR, // "-"
  MULTIPLICATION_OPERATOR, // "*"
  DIVISION_OPERATOR, // "/"
  GREATER_THAN_OPERATOR, // ">"
  LESS_THAN_OPERATOR, // "<"
  GREATER_THAN_OR_EQUAL_TO_OPERATOR, // ">="
  LESS_THAN_OR_EQUAL_TO_OPERATOR, // "<="
  EQUAL_TO_OPERATOR, // "=="
  NOT_EQUAL_TO_OPERATOR, // "!="
  COMMA_OPERATOR, // ","
  LEFT_PARENTHESIS, // "("
  RIGHT_PARENTHESIS, // ")"
  LEFT_CURLY_BRACKET, // "{"
  RIGHT_CURLY_BRACKET, // "}"
  SEMICOLON, // ";"
  END_OF_FILE // -1
} type_t;

struct Token
{
  type_t type;
  char *data;
};

struct Token *get_token();

#endif
