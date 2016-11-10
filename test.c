#include "token.h"

char *get_enumerator_name(type_t value)
{
  switch (value)
  {
    case KEYWORD: return "KEYWORD"; // "break", "class", "continue", "do", "else", "false", "for", "if", "return", "static", "true", "void", "while"
    case IDENTIFIER: return "IDENTIFIER"; //
    case BOOLEAN_DATA_TYPE: return "BOOLEAN_DATA_TYPE"; // boolean
    case INT_DATA_TYPE: return "INT_DATA_TYPE"; // int
    case DOUBLE_DATA_TYPE: return "DOUBLE_DATA_TYPE"; // double
    case STRING_DATA_TYPE: return "STRING_DATA_TYPE"; // String
    case INT_LITERAL: return "INT_LITERAL"; //
    case DOUBLE_LITERAL: return "DOUBLE_LITERAL"; //
    case STRING_LITERAL: return "STRING_LITERAL"; //
    case ASSIGNMENT_OPERATOR: return "ASSIGNMENT_OPERATOR"; // "="
    case ADDITION_OPERATOR: return "ADDITION_OPERATOR"; // "+"
    case SUBTRACTION_OPERATOR: return "SUBTRACTION_OPERATOR"; // "-"
    case MULTIPLICATION_OPERATOR: return "MULTIPLICATION_OPERATOR"; // "*"
    case DIVISION_OPERATOR: return "DIVISION_OPERATOR"; // "/"
    case GREATER_THAN_OPERATOR: return "GREATER_THAN_OPERATOR"; // ">"
    case LESS_THAN_OPERATOR: return "LESS_THAN_OPERATOR"; // "<"
    case GREATER_THAN_OR_EQUAL_TO_OPERATOR: return "GREATER_THAN_OR_EQUAL_TO_OPERATOR"; // ">="
    case LESS_THAN_OR_EQUAL_TO_OPERATOR: return "LESS_THAN_OR_EQUAL_TO_OPERATOR"; // "<="
    case EQUAL_TO_OPERATOR: return "EQUAL_TO_OPERATOR"; // "=="
    case NOT_EQUAL_TO_OPERATOR: return "NOT_EQUAL_TO_OPERATOR"; // "!="
    case COMMA_OPERATOR: return "COMMA_OPERATOR"; // ","
    case LEFT_PARENTHESIS: return "LEFT_PARENTHESIS"; // "("
    case RIGHT_PARENTHESIS: return "RIGHT_PARENTHESIS"; // ")"
    case LEFT_CURLY_BRACKET: return "LEFT_CURLY_BRACKET"; // "{"
    case RIGHT_CURLY_BRACKET: return "RIGHT_CURLY_BRACKET"; // "}"
    case SEMICOLON: return "SEMICOLON"; // ";"
    case END_OF_FILE: return "END_OF_FILE"; // -1
  }
  return NULL;
}

