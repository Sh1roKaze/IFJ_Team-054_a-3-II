#include "test.h"

int main()
{
  testLexicalAnalysis();

  return 0;
}

char *getEnumeratorName(TokenType type)
{
  switch (type)
  {
    case NOT_EQUAL_TO_OPERATOR: return "NOT_EQUAL_TO_OPERATOR"; // "!="
    case LEFT_PARENTHESIS: return "LEFT_PARENTHESIS"; // "("
    case RIGHT_PARENTHESIS: return "RIGHT_PARENTHESIS"; // ")"
    case MULTIPLICATION_OPERATOR: return "MULTIPLICATION_OPERATOR"; // "*"
    case ADDITION_OPERATOR: return "ADDITION_OPERATOR"; // "+"
    case COMMA_OPERATOR: return "COMMA_OPERATOR"; // ","
    case SUBTRACTION_OPERATOR: return "SUBTRACTION_OPERATOR"; // "-"
    case DIVISION_OPERATOR: return "DIVISION_OPERATOR"; // "/"
    case SEMICOLON: return "SEMICOLON"; // ";"
    case LESS_THAN_OPERATOR: return "LESS_THAN_OPERATOR"; // "<"
    case LESS_THAN_OR_EQUAL_TO_OPERATOR: return "LESS_THAN_OR_EQUAL_TO_OPERATOR"; // "<="
    case ASSIGNMENT_OPERATOR: return "ASSIGNMENT_OPERATOR"; // "="
    case EQUAL_TO_OPERATOR: return "EQUAL_TO_OPERATOR"; // "=="
    case GREATER_THAN_OPERATOR: return "GREATER_THAN_OPERATOR"; // ">"
    case GREATER_THAN_OR_EQUAL_TO_OPERATOR: return "GREATER_THAN_OR_EQUAL_TO_OPERATOR"; // ">="
    case LEFT_CURLY_BRACKET: return "LEFT_CURLY_BRACKET"; // "{"
    case RIGHT_CURLY_BRACKET: return "RIGHT_CURLY_BRACKET"; // "}"
    case BOOLEAN_DATA_TYPE: return "BOOLEAN_DATA_TYPE"; // "boolean"
    case BREAK_KEYWORD: return "BREAK_KEYWORD"; // "break"
    case CLASS_KEYWORD: return "CLASS_KEYWORD"; // "class"
    case CONTINUE_KEYWORD: return "CONTINUE_KEYWORD"; // "continue"
    case DO_KEYWORD: return "DO_KEYWORD"; // "do"
    case DOUBLE_DATA_TYPE: return "DOUBLE_DATA_TYPE"; // "double"
    case ELSE_KEYWORD: return "ELSE_KEYWORD"; // "else"
    case FALSE_KEYWORD: return "FALSE_KEYWORD"; // "false"
    case FOR_KEYWORD: return "FOR_KEYWORD"; // "for"
    case IF_KEYWORD: return "IF_KEYWORD"; // "if"
    case INT_DATA_TYPE: return "INT_DATA_TYPE"; // "int"
    case RETURN_KEYWORD: return "RETURN_KEYWORD"; // "return"
    case STRING_DATA_TYPE: return "STRING_DATA_TYPE"; // "String"
    case STATIC_KEYWORD: return "STATIC_KEYWORD"; // "static"
    case TRUE_KEYWORD: return "TRUE_KEYWORD"; // "true"
    case VOID_KEYWORD: return "VOID_KEYWORD"; // "void"
    case WHILE_KEYWORD: return "WHILE_KEYWORD"; // "while"
    case IDENTIFIER: return "IDENTIFIER"; // "^[a-zA-Z_$][a-zA-Z0-9_$]*([.][a-zA-Z_$][a-zA-Z0-9_$]*)?$"
    case INT_LITERAL: return "INT_LITERAL"; // "^[0]*[1-9][0-9]*$"
    case DOUBLE_LITERAL: return "DOUBLE_LITERAL"; // "^[0-9]+[.][0-9]+([eE][+-]?[0-9]+)?$"
    case STRING_LITERAL: return "STRING_LITERAL"; // "^\".*\"$"
    case END_OF_FILE: return "END_OF_FILE"; // -1
  }
  return NULL;
}

Token *newToken(TokenType type, char *data)
{
  Token *t = getToken();
  t->type = type;
  t->data = data;

  return t;
}

void printToken(Token *t)
{
  printf("Token:\ntype: %s\ndata: %s\n\n", getEnumeratorName(t->type), t->data);
  return;
}

int stringToInteger(char *inputString)
{
  long int castedInteger;
  char *endPointer;
  errno = 0;

  if (inputString[0] == '0')
  {
    if (inputString[1] != '\0')
      castedInteger = strtol(inputString, &endPointer, 8);
    else
      return 0;
  }
  else
    castedInteger = strtol(inputString, &endPointer, 10);

  if (castedInteger < 0 || errno != 0 || castedInteger > INT_MAX || endPointer[0] != '\0')
    return -1;
  return (int)castedInteger;
}

void testBuffer(Buffer **b, char *bufferName)
{
  int allocationOfBufferSucceeded = 0;

  printf("[Test] Buffer test:\n");

  if (*b != NULL)
  {
    printf("[ OK ] Buffer \"%s\" was successfully allocated to the size of %u bytes\n", bufferName, (*b)->size * (*b)->multiplier);
    allocationOfBufferSucceeded++;
  }
  else
    printf("[Fail] Buffer \"%s\" wasn't successfully allocated\n", bufferName);

  disposeBuffer(b);

  if (allocationOfBufferSucceeded)
  {
    if (*b == NULL)
      printf("[ OK ] Buffer \"%s\" was successfully deallocated\n", bufferName);
    else
      printf("[Fail] Buffer \"%s\" wasn't successfully deallocated\n", bufferName);
  }

  printf("\n");

  return;
}

void testLexicalAnalysis()
{
  int i = 0;
  Token *tokenArray[500];

  beginLexicalAnalysis();

  do
  {
    tokenArray[i] = getToken();
    printf("\n----------------------------------\n");
    printf("Token[%d]:\ntype: %s\ndata: %s\n", i, getEnumeratorName(tokenArray[i]->type), tokenArray[i]->data);
    printf("----------------------------------\n\n");
  }
  while (tokenArray[i++]->type != END_OF_FILE);

  return;
}

void testPointerSizes()
{
  char comparisonSign;

  printf("[Test] Void pointer size test:\n");

  // int
  if (sizeof(void *) > sizeof(int)) comparisonSign = '>';
  else if (sizeof(void *) == sizeof(int)) comparisonSign = '=';
  else comparisonSign = '<';
  (sizeof(void *) >= sizeof(int)) ? printf("[ OK ]") : printf("[FAIL]");
  printf(" \"void *\" (%lu bytes) %c \"int\" (%lu bytes)\n", sizeof(void *), comparisonSign, sizeof(int));

  // int *
  if (sizeof(void *) > sizeof(int *)) comparisonSign = '>';
  else if (sizeof(void *) == sizeof(int *)) comparisonSign = '=';
  else comparisonSign = '<';
  (sizeof(void *) >= sizeof(int *)) ? printf("[ OK ]") : printf("[FAIL]");
  printf(" \"void *\" (%lu bytes) %c \"int *\" (%lu bytes)\n", sizeof(void *), comparisonSign, sizeof(int *));

  // double
  if (sizeof(void *) > sizeof(double)) comparisonSign = '>';
  else if (sizeof(void *) == sizeof(double)) comparisonSign = '=';
  else comparisonSign = '<';
  (sizeof(void *) >= sizeof(double)) ? printf("[ OK ]") : printf("[FAIL]");
  printf(" \"void *\" (%lu bytes) %c \"double\" (%lu bytes)\n", sizeof(void *), comparisonSign, sizeof(double));

  //double *
  if (sizeof(void *) > sizeof(double *)) comparisonSign = '>';
  else if (sizeof(void *) == sizeof(double *)) comparisonSign = '=';
  else comparisonSign = '<';
  (sizeof(void *) >= sizeof(double *)) ? printf("[ OK ]") : printf("[FAIL]");
  printf(" \"void *\" (%lu bytes) %c \"double *\" (%lu bytes)\n", sizeof(void *), comparisonSign, sizeof(double *));

  //char
  if (sizeof(void *) > sizeof(char)) comparisonSign = '>';
  else if (sizeof(void *) == sizeof(char)) comparisonSign = '=';
  else comparisonSign = '<';
  (sizeof(void *) >= sizeof(char)) ? printf("[ OK ]") : printf("[FAIL]");
  printf(" \"void *\" (%lu bytes) %c \"char\" (%lu bytes)\n", sizeof(void *), comparisonSign, sizeof(char));

  //char *
  if (sizeof(void *) > sizeof(char *)) comparisonSign = '>';
  else if (sizeof(void *) == sizeof(char *)) comparisonSign = '=';
  else comparisonSign = '<';
  (sizeof(void *) >= sizeof(char *)) ? printf("[ OK ]") : printf("[FAIL]");
  printf(" \"void *\" (%lu bytes) %c \"char *\" (%lu bytes)\n", sizeof(void *), comparisonSign, sizeof(char *));

  printf("\n");

  return;
}

void testToken()
{
  char *integerLiteral = "42";
  char *doubleLiteral = "3.1415";
  char *stringLiteral = "This is a string!";
  Token *t[ARRAY_SIZE];

  t[0] = newToken(INT_LITERAL, integerLiteral);
  t[1] = newToken(DOUBLE_LITERAL, doubleLiteral);
  t[2] = newToken(STRING_LITERAL, stringLiteral);

  for (int i = 0; i < ARRAY_SIZE; i++)
    printf("type: %s\n", getEnumeratorName(t[i]->type));
  printf("data: %s\n", (char *)t[0]->data);
  printf("data: %s\n", (char *)t[1]->data);
  printf("data: %s\n", (char *)t[2]->data);

  printf("\n");

  for (int i = 0; i < ARRAY_SIZE; i++)
    free(t[i]);

  return;
}
