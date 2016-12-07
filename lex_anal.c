#include "lex_anal.h"

// global variables
Buffer *mainBuffer = NULL; // is used to store characters which have already been read
Token *currentToken = NULL; // will be initialized when the final state is reached
State (*currentState)(); // points to the function corrensponding to the current state
int transferedCharacter; // will be used to store next character when the final state is reached
int currentCharacter; // holds last read character
int currentDatalessTokenType; // holds number of current dataless token or equals to -1 otherwise
int endOfTokenFlag; // indicates, if the final state was reached (positive integer) or not (0)
int errorCode; // specifies which error has occured, if there has been one
const char *const datalessTokenType[] = {
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
  "" // lame but stupidly elegant segmentation fault fix :D
};

// functions - buffer ADT
void addCharacterToBuffer(Buffer *b, char c) {
  b->array[b->position] = c;
  b->position++;
  return;
}

void addCurrentCharacterToBuffer(Buffer *b) {
  addCharacterToBuffer(b, currentCharacter);
  return;
}

char *copyBufferArray(Buffer *b) {
  addCharacterToBuffer(b, '\0');
  char *string = (char *)malloc(b->position);
  if (string == NULL)
    printErrorMessageAndPrepareForExiting("malloc failure", 99);
  else
    strcpy(string, b->array);
  return string;
}

void disposeBuffer(Buffer **b) {
  if (*b != NULL) {
    free((*b)->array);
    free(*b);
    *b = NULL;
  }
  return;
}

int extendBufferArray(Buffer *b) {
  b->multiplier <<= 1;
  b->array = (char *)realloc(b->array, b->size * b->multiplier);
  if (b->array != NULL)
    return 1;
  printErrorMessageAndPrepareForExiting("realloc failure", 99);
  return -1;
}

Buffer *initializeBuffer(unsigned int size) {
  Buffer *b = (Buffer *)malloc(sizeof(Buffer));
  if (b != NULL) {
    b->size = size;
    b->multiplier = 1;
    b->position = 0;
    b->array = (char *)malloc(b->size * b->multiplier);
    if (b->array != NULL)
      return b;
  }
  printErrorMessageAndPrepareForExiting("malloc failure", 99);
  exit(errorCode);
}

int isBufferEmpty(Buffer *b) {
  return (b->position == 0);
}

void resetBufferPosition(Buffer *b) {
  b->position = 0;
  return;
}

// functions - FSM states
State datalessType() { //printf("[currentState] DATALESS TYPE: %c | %x | %d | %c | %x | %lx\n", currentCharacter, currentCharacter, mainBuffer->position, datalessTokenType[currentDatalessTokenType][mainBuffer->position], datalessTokenType[currentDatalessTokenType][mainBuffer->position], strlen(datalessTokenType[currentDatalessTokenType]));
  if (currentCharacter != datalessTokenType[currentDatalessTokenType][mainBuffer->position]) { // dataless type continues with unexpected character
    if (mainBuffer->position == 1 && currentDatalessTokenType == DIVISION_OPERATOR) { // division operator occurance could also mean beginning of comment
        divisionOperatorHelpingFunction(); // slash -> ignoreLine; asterisk -> ignoreBlock; everything else -> end + transfer
        return;
    }
    else if (mainBuffer->position == 1 && currentDatalessTokenType == MULTIPLICATION_OPERATOR && currentCharacter == '/') { // multiplication operator occurance could also mean possible termination of non-existing block comment
      printErrorMessageAndPrepareForExiting("termination of non-existing block comment", 1);
      return;
    }
    else if (mainBuffer->position == 1 && datalessTokenType[currentDatalessTokenType][0] == datalessTokenType[currentDatalessTokenType + 1][0] && currentCharacter == datalessTokenType[currentDatalessTokenType + 1][1]) // verifying second dataless type string with the same first character
      currentDatalessTokenType++;
    else if (currentDatalessTokenType == DO_KEYWORD && mainBuffer->position == 2 && currentCharacter == 'u') // do keyword is contained in double keyword, therefore special verification is essential
      currentDatalessTokenType++;
    else if (currentDatalessTokenType > RIGHT_CURLY_BRACKET) {
      if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1) {
        transferedCharacter = currentCharacter;
        if (mainBuffer->position < strlen(datalessTokenType[currentDatalessTokenType]))
          endCurrentTokenWithData(IDENTIFIER);
        else
          endCurrentToken(currentDatalessTokenType);
        return;
      }
      else if (currentCharacterMatchIdentifierNextLetter())
        currentState = &identifier;
      else if (currentCharacter == '.')
        currentState = &identifierDot;
      else { // this should probably never happen
        printErrorMessageAndPrepareForExiting("no keyword matched", 1);
        return;
      }
    }
    else { // it's neither dataless type nor identifier (it will be analyzed in next getToken() function call
      transferedCharacter = currentCharacter;
      endCurrentToken(currentDatalessTokenType);
      return;
    }
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralDot() { //printf("[currentState] DOUBLE LITERAL DOT: %c\n", currentCharacter);
  if (currentCharacterMatchRange('0', '9'))
    currentState = &doubleLiteralFraction;
  else {
    printErrorMessageAndPrepareForExiting("invalid double format", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralExponent() { //printf("[currentState] DOUBLE LITERAL EXPONENT: %c\n", currentCharacter);
  if (currentCharacter == '-' || currentCharacter == '+')
    currentState = &doubleLiteralSign;
  else if (currentCharacterMatchRange('0', '9'))
    currentState = &doubleLiteralFinish;
  else {
    printErrorMessageAndPrepareForExiting("invalid double format", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralFinish() { //printf("[currentState] DOUBLE LITERAL FINISH: %c\n", currentCharacter);
  if (!currentCharacterMatchRange('0', '9')) {
    if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1) {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(DOUBLE_LITERAL);
      return;
    }
    else {
      printErrorMessageAndPrepareForExiting("invalid double format", 1);
      return;
    }
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralFraction() { //printf("[currentState] DOUBLE LITERAL FRACTION: %c\n", currentCharacter);
  if (!currentCharacterMatchRange('0', '9')) {
    if (currentCharacter == 'e' || currentCharacter == 'E')
      currentState = &doubleLiteralExponent;
    else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1) {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(DOUBLE_LITERAL);
      return;
    }
    else {
      printErrorMessageAndPrepareForExiting("invalid double format", 1);
      return;
    }
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State doubleLiteralSign() { //printf("[currentState] DOUBLE LITERAL SIGN: %c\n", currentCharacter);
  if (currentCharacterMatchRange('0', '9'))
    currentState = &doubleLiteralFinish;
  else {
    printErrorMessageAndPrepareForExiting("invalid double format", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State identifier() { //printf("[currentState] IDENTIFIER: %c\n", currentCharacter);
  identifierHelpingFunction(1); // next letter -> identifier; dot -> identifierDot; whitespace/beginning of operator -> end + transfer; everything else -> fail
  return;
}

State identifierDot() { //printf("[currentState] IDENTIFIER DOT: %c\n", currentCharacter);
  if (currentCharacterMatchIdentifierFirstLetter())
    currentState = &identifierFinish;
  else {
    printErrorMessageAndPrepareForExiting("invalid identifier format", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State identifierFinish() { //printf("[currentState] IDENTIFIER FINISH: %c\n", currentCharacter);
  identifierHelpingFunction(0); // next letter -> identifierFinish; whitespace/beginning of operator -> end + transfer; everything else -> fail
  return;
}

State ignoreBlock() { //printf("[currentState] IGNORE BLOCK: %c\n", currentCharacter);
  if (currentCharacter == '*')
    currentState = &ignoreBlockAsterisk;
  return;
}

State ignoreBlockAsterisk() { //printf("[currentState] IGNORE BLOCK ASTERISK: %c\n", currentCharacter);
  if (currentCharacter == '/')
    currentState = &start;
  else
    currentState = &ignoreBlock;
  return;
}

State ignoreLine() { //printf("[currentState] IGNORE LINE: %c\n", currentCharacter);
  if (currentCharacter == '\n')
    currentState = &start;
  return;
}

State numberLiteral() { //printf("[currentState] NUMBER LITERAL: %c\n", currentCharacter);
  if (!currentCharacterMatchRange('0', '9')) {
    if (currentCharacter == '.')
      currentState = &doubleLiteralDot;
    else if (currentCharacter == 'e' || currentCharacter == 'E')
      currentState = &doubleLiteralExponent;
    else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1) {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(INT_LITERAL);
      return;
    }
    else {
      printErrorMessageAndPrepareForExiting("invalid integer format", 1);
      return;
    }
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State start() { //printf("[currentState] START: %x | %c\n", currentCharacter, currentCharacter);
  if (currentCharacter == currentCharacterMatchWhitespace())
    return;
  else if (currentCharacter == '\"') {
    currentState = &stringLiteral;
    return;
  }
  else if ((currentDatalessTokenType = currentCharacterMatchDatalessTokenType()) != -1)
    currentState = &datalessType;
  else if (currentCharacterMatchRange('0', '9'))
    currentState = &numberLiteral;
  else if (currentCharacterMatchString("_$aghjklmnopqruxyzABCDEFGHIJKLMNOPQRTUVWXYZ")) //first letters of keywords omitted
    currentState = &identifier;
  else {
    printErrorMessageAndPrepareForExiting("invalid lexeme format", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State stringLiteral() { //printf("[currentState] STRING LITERAL: %c\n", currentCharacter);
  if (currentCharacter == '\\') {
    currentState = &stringLiteralBackslash;
    return;
  }
  else if (currentCharacter == '\"') {
    endCurrentTokenWithData(STRING_LITERAL);
    return;
  }
  else if (currentCharacter == '\n') {
    printErrorMessageAndPrepareForExiting("string exceeds single line", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

State stringLiteralBackslash() { //printf("[currentState] STRING LITERAL BACKSLASH: %c\n", currentCharacter);
  if (currentCharacterMatchRange('0', '3')) {
    addCurrentCharacterToBuffer(mainBuffer);
    currentState = &stringLiteralOctalEscapeSequence;
    return;
  }
  else if (currentCharacter == '\"' || currentCharacter == '\\')
    addCurrentCharacterToBuffer(mainBuffer);
  else if (currentCharacter == 'n')
    addCharacterToBuffer(mainBuffer, '\n');
  else if (currentCharacter == 't')
    addCharacterToBuffer(mainBuffer, '\t');
  else {
    printErrorMessageAndPrepareForExiting("string contains invalid escape sequence", 1);
    return;
  }
  currentState = &stringLiteral;
  return;
}

State stringLiteralOctalEscapeSequence() { //printf("[currentState] STRING LITERAL OCTAL ESCAPE SEQUENCE: %c\n", currentCharacter);
  if (!currentCharacterMatchRange('0', '7')) {
    printErrorMessageAndPrepareForExiting("string contains invalid escape sequence", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  currentState = &stringLiteralOctalEscapeSequenceFinish;
  return;
}

State stringLiteralOctalEscapeSequenceFinish() { //printf("[currentState] STRING LITERAL OCTAL ESCAPE SEQUENCE FINISH: %c\n", currentCharacter);
  if (!currentCharacterMatchRange('0', '7') || (currentCharacter == '0' && mainBuffer->array[mainBuffer->position - 1] == '0' && mainBuffer->array[mainBuffer->position - 2] == '0')) {
    printErrorMessageAndPrepareForExiting("string contains invalid escape sequence", 1);
    return;
  }
  addCurrentCharacterToBuffer(mainBuffer);
  addCharacterToBuffer(mainBuffer, '\0');
  mainBuffer->position -= 4;
  addCharacterToBuffer(mainBuffer, strtol(mainBuffer->array + mainBuffer->position, NULL, 8));
  currentState = &stringLiteral;
  return;
}

// functions - helping functions
void beginLexicalAnalysis() {
  mainBuffer = initializeBuffer(BUFFER_SIZE); // recommended BUFFER_SIZE is 1024 (defined in header)
  return;
}

void divisionOperatorHelpingFunction() {
  if (currentCharacter == '/')
    currentState = &ignoreLine;
  else if (currentCharacter == '*')
    currentState = &ignoreBlock;
  else {
    transferedCharacter = currentCharacter;
    endCurrentToken(currentDatalessTokenType);
    return;
  }
  mainBuffer->position--;
  return;
}

void endCurrentToken(TokenType type) {
  currentToken = initializeToken(type, NULL);
  endOfTokenFlag = 1;
  return;
}

void endCurrentTokenWithData(TokenType type) {
  char *data = copyBufferArray(mainBuffer);
  if (data == NULL)
    return;
  currentToken = initializeToken(type, data);
  endOfTokenFlag = 1;
  return;
}

void endLexicalAnalysis() {
  disposeBuffer(&mainBuffer);
  return;
}

void identifierHelpingFunction(int acceptDotFlag) {
  if (!currentCharacterMatchIdentifierNextLetter()) {
    if (currentCharacter == '.' && acceptDotFlag)
      currentState = &identifierDot;
    else if (currentCharacterMatchWhitespace() || currentCharacterMatchOperatorTokenType() != -1) {
      transferedCharacter = currentCharacter;
      endCurrentTokenWithData(IDENTIFIER);
      return;
    }
    else {
      printErrorMessageAndPrepareForExiting("invalid identifier format", 1);
      return;
    }
  }
  addCurrentCharacterToBuffer(mainBuffer);
  return;
}

void printErrorMessageAndPrepareForExiting(char *errorMessage, int returnValue) {
  endLexicalAnalysis(); // freeing of mainBuffer
  fprintf(stderr, "ERROR: %s!\n", errorMessage);
  currentToken = NULL;
  endOfTokenFlag = 1;
  errorCode = returnValue;
  return;
}

// functions - regex substitution
int currentCharacterMatchDatalessTokenType() {
  int returnCode;

  if ((returnCode = currentCharacterMatchOperatorTokenType()) == -1)
    if ((returnCode = currentCharacterMatchKeywordTokenType()) == -1)
      return -1;
  return returnCode;
}

int currentCharacterMatchIdentifierFirstLetter() {
  return (currentCharacterMatchRange('a', 'z') || currentCharacterMatchRange('A', 'Z') || currentCharacterMatchString("_$"));
}

int currentCharacterMatchIdentifierNextLetter() {
  return (currentCharacterMatchIdentifierFirstLetter() || currentCharacterMatchRange('0', '9'));
}

int currentCharacterMatchKeywordTokenType() {
  for (int i = BOOLEAN_DATA_TYPE; i < WHILE_KEYWORD + 1; i++)
    if (currentCharacter == datalessTokenType[i][0])
      return i;
  return -1;
}

int currentCharacterMatchOperatorTokenType() {
  for (int i = NOT_EQUAL_TO_OPERATOR; i < BOOLEAN_DATA_TYPE; i++)
    if (currentCharacter == datalessTokenType[i][0])
      return i;
  return -1;
}

char currentCharacterMatchRange(char first, char last) {
  for (int i = first; i < last + 1; i++)
    if (currentCharacter == i)
      return currentCharacter;
  return '\0';
}

char currentCharacterMatchString(char *string) {
  for (int i = 0; string[i] != '\0'; i++)
    if (currentCharacter == string[i])
      return currentCharacter;
  return '\0';
}

char currentCharacterMatchWhitespace() {
  return currentCharacterMatchString(" \n\t\r"); // remove '\r' !!!!!!
}

// functions - token ADT
void disposeToken(Token **t) {
  if (*t != NULL) {
    free((*t)->data);
    free(*t);
    *t = NULL;
  }
  return;
}

Token *getToken() {
  resetBufferPosition(mainBuffer);
  currentState = &start;
  endOfTokenFlag = 0;

  if (transferedCharacter != -1) { // verifies, if a character left from the last getToken call is transfered into the current function run
    currentCharacter = transferedCharacter;
    currentState();
    transferedCharacter = -1;
  }

  while (1) { // it is very unlikely for the buffer to be reallocated so many times it would cause an overflow (but its' not entirely bad idea to take care of it some other day in the future)
    while (mainBuffer->position < mainBuffer->size * mainBuffer->multiplier - 1) { // since currentCharacter is expected to fill current mainBuffer's array position and at any point the end of currentToken could be reached, at least the last position of the mainBuffer's array must not be used, in order to be filled with '\0', if needed
      if ((currentCharacter = fgetc(stdin)) != EOF) {
        currentState();
        if (endOfTokenFlag)
          return currentToken;
      }
      else if (currentState != ignoreBlock && currentState != ignoreBlockAsterisk) {
        if (mainBuffer->position == 0) { // a valid source code must always have a '}' before the end-of-file
          endCurrentToken(END_OF_FILE);
          endLexicalAnalysis(); // freeing of mainBuffer
          return currentToken;
        }
        else { // EOF reached in the middle of a token
          printErrorMessageAndPrepareForExiting("unexpected end-of-file", 1);
          return NULL;
        }
      }
      else {
        printErrorMessageAndPrepareForExiting("unterminated block comment", 1);
        return NULL;
      }
    }
    if (extendBufferArray(mainBuffer) == -1) // reallocation of mainBuffer's array (some identifier/literal must be pretty long!)
      return NULL;
  }
}

Token *initializeToken(TokenType type, char *data) {
  Token *t = (Token *)malloc(sizeof(Token));
  if (t != NULL) {
    t->type = type;
    t->data = data;
  }
  else
    printErrorMessageAndPrepareForExiting("malloc failure", 99);
  return t;
}
