/* header */

#include "token.h"

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
    COMPARISON   
} node_t;

typedef struct tTNode	{                              
    node_t key;
    char *literal;
    struct tTNode *LPtr;
    struct tTNode *RPtr; 	  	                         
} *tTNodePtr;


/* prototypes */

tTNodePtr derivationTree; //Root of derivation tree
struct Token *token; //global token variable
int ret; //global return variable

int parser();
