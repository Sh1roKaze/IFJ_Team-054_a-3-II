/* header */

typedef struct tTNode	{                              
    type_t key;                                         
    struct tTNode LPtr;
    struct tTNode MPtr;
    struct tTNode RPtr; 	  	                         
} *tTNodePtr; 

/* prototypes */

tTNodePtr derivationTree; //Root of derivation tree
token_t token; //global token variable
int ret; //global return variable

int parser();
