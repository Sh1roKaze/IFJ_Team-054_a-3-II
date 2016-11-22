#ifndef LEX_ANAL_H
#define LEX_ANAL_H

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "error.h"
#include "match.h"
#include "token.h"

#define BUFFER_SIZE 1024

typedef void State;

void beginLexicalAnalysis();
State datalessType();
void divisionOperatorHelpingFunction();
State doubleLiteralDot();
State doubleLiteralExponent();
State doubleLiteralFinish();
State doubleLiteralFraction();
State doubleLiteralSign();
void endCurrentTokenWithData(TokenType type);
void endCurrentToken(TokenType type);
void endLexicalAnalysis();
State endOfFile();
State identifier();
State identifierDot();
State identifierFinish();
void identifierHelpingFunction(int);
State ignoreBlock();
State ignoreBlockAsterisk();
State ignoreLine();
State numberLiteral();
State start();
State stringLiteral();
State stringLiteralBackslash();
State stringLiteralOctalEscapeSequence();

#endif
