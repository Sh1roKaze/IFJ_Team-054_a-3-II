/* header */

typedef struct tTNode	{                              
    char *key;                                         
    struct tTNode LPtr;
    struct tTNode MPtr;
    struct tTNode RPtr; 	  	                         
} *tTNodePtr; 

/* prototypes */

tTNodePtr derivationTree;

int parser();
