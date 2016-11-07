/* header */

typedef struct tBTNode	{                              
    char *key;                                         
    struct tBTNode * LPtr;                               
    struct tBTNode * RPtr; 	  	                         
} *tBTNodePtr; 

typedef struct tLLNode {
    char *name;
    struct tLLNode *next;
} *tLLNodePtr;

typedef struct tLList {
    tLLNodePtr head;
    tLLNodePtr active;
} *tLLPtr;


/* prototypes */
int parser();