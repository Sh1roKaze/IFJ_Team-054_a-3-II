/*
Projekt IFJ16

xverny00 - Jan Verny
*/

/* header */
#ifndef PARSER_H
#define PARSER_H

#include "lex_anal.h"

typedef enum {
    CLASS_LIST,
    CLASS,
    CLASS_ITEM,
    FUNCTION,
    STATIC_VAR,
    DECLARATION,
    ARG_LIST,
    ARG,
    ST_LIST,
    LOCAL_VAR,
    STATEMENT,
    ASSIGNMENT,
    BLOCK,
    BLOCK_LIST,
    CONDITION,
    CYCLE,
    CALL,
    PAR_LIST,
    PAR,
    RETURN,
    EXPRESSION,
    TERM,
    FACTOR,
    STRING,
    INT,
    DOUBLE,
    INT_DATA,
    STRING_DATA,
    DOUBLE_DATA,
    VOID_DATA,
    ID,
    COMPARISON,
    UNINITIALIZED   
} node_t;

typedef struct tTNode	{                              
    node_t key;
    char *literal;
    struct tTNode *LPtr;
    struct tTNode *RPtr; 	  	                         
} *tTNodePtr;


/* prototypes */

tTNodePtr derivationTree; //Root of derivation tree
Token *token; //global token variable
int ret; //global return variable

void freeTree(tTNodePtr *tree);
int file(tTNodePtr tree);
void printTree(tTNodePtr tree);
char *getNodeString(node_t type); 

#endif
