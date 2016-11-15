/* header */
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
    STRING_LITERAL
    INT_LITERAL,
    DOUBLE_LITERAL,
    INT_DATA_TYPE,
    STRING_DATA_TYPE,
    DOUBLE_DATA_TYPE,
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
token_t token; //global token variable
int ret; //global return variable

int parser();
