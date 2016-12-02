#include "parser.h"
#include "token.h"
#include <string.h>

 int file(tTNodePtr tree);
 int classList(tTNodePtr tree);
 int class(tTNodePtr tree);
 int classitem(tTNodePtr tree);
 int function(tTNodePtr tree, tTNodePtr temp);
 int staticvar(tTNodePtr tree, tTNodePtr temp);
 int declaration(tTNodePtr tree);
 int arglist(tTNodePtr tree);
 int argument(tTNodePtr tree);
 int stlist(tTNodePtr tree);
 int localvar(tTNodePtr tree);
 int statement(tTNodePtr tree);
 int assignment(tTNodePtr tree, tTNodePtr temp);
 int block(tTNodePtr tree);
 int blocklist(tTNodePtr tree);
 int condition(tTNodePtr tree);
 int cycle(tTNodePtr tree);
 int call(tTNodePtr tree);
 int parlist(tTNodePtr tree);
 int parameter(tTNodePtr tree);
 int retval(tTNodePtr tree);
 int expression(tTNodePtr tree);
 int term (tTNodePtr tree);
 int safeExpression(tTNodePtr tree);
 int factor(tTNodePtr tree);    
 int literal(tTNodePtr tree);
 int id(tTNodePtr tree);
 int type(tTNodePtr tree);
 int comparison(tTNodePtr tree);

 inline void treeInit(tTNodePtr tree) {
     tree = NULL;
 }


 char *getNodeString(node_t type) {
     switch (type) {
     case CLASS_LIST: return "CLASS_LIST";
     case CLASS: return "CLASS";
     case CLASS_ITEM: return "CLASS_ITEM";
     case FUNCTION: return "FUNCTION";
     case STATIC_VAR: return "STATIC_VAR";
     case DECLARATION: return "DECLARATION";
     case ARG_LIST: return "ARG_LIST";
     case ARG: return "ARG";
     case ST_LIST: return "ST_LIST";
     case LOCAL_VAR: return "LOCAL_VAR";
     case STATEMENT: return "STATEMENT";
     case ASSIGNMENT: return "ASSIGNMENT";
     case BLOCK: return "BLOCK";
     case BLOCK_LIST: return "BLOCK_LIST";
     case CONDITION: return "CONDITION";
     case CYCLE: return "CYCLE";
     case CALL: return "CALL";
     case PAR_LIST: return "PAR_LIST";
     case PAR: return "PAR";
     case RETURN: return "RETURN";
     case EXPRESSION: return "EXPRESSION";
     case TERM: return "TERM";
     case FACTOR: return "FACTOR";
     case STRING: return "STRING";
     case INT: return "INT";
     case DOUBLE: return "DOUBLE";
     case INT_DATA: return "INT_DATA";
     case STRING_DATA: return "STRING_DATA";
     case DOUBLE_DATA: return "DOUBLE_DATA";
     case VOID_DATA: return "VOID_DATA";
     case ID: return "ID";
     case COMPARISON: return "COMPARISON"; 
     }
 }


 void freeTree(tTNodePtr *tree) {
     if (tree != NULL && *tree != NULL) {
	     if ((*tree)->LPtr != NULL) {
		  freeTree(&((*tree)->LPtr));
	     }

	     if ((*tree)->RPtr != NULL) {
	          freeTree(&((*tree)->RPtr));
	     }

             free((*tree)->literal);
             free(*tree);
             *tree = NULL;
     }
 } 


 void printTree(tTNodePtr tree) {
     if (tree != NULL ) {
             printf("<%s> ", getNodeString(tree->key));
             if (tree->literal != NULL ) {
                  printf("%s\n", tree->literal);
             } else {
                  printf("\n");
             }
	     if (tree->LPtr != NULL) {
		  printTree(tree->LPtr);
	     }

	     if (tree->RPtr != NULL) {
	          printTree(tree->RPtr);
	     }
     }
 
 }


 int accept(TokenType sym) {
     if (token != NULL) {
         return (int) (token->type == sym);
     } else {
         return 0;
     }
 }


 int file(tTNodePtr tree) {
     tree->LPtr = NULL;
     tree->RPtr = NULL;
     ret =  classList(tree); 
     if (token == NULL) {
         return 1;
     } 
     if (ret == 2) { 
         return 2;
     }
     return 0;
 }


 int classList(tTNodePtr tree) {
     tree->key = CLASS_LIST;
     tree->literal = NULL;

     token = getToken();
     if (accept(END_OF_FILE)) {
         return 2;
     }
     
     tree->LPtr = malloc(sizeof(struct tTNode));
     ret = class(tree->LPtr);
     if (ret == 2) {
         return 2;
     }

     while (1) {
         token = getToken(); 
         if (accept(END_OF_FILE)) {
            return 0;
         }

         tree->RPtr = malloc(sizeof(struct tTNode)); 
         tree = tree->RPtr;
         tree->key = CLASS_LIST;
         tree->literal = NULL;

         tree->LPtr = malloc(sizeof(struct tTNode));
         ret = class(tree->LPtr);
         if (ret == 2) {
            return 2; 
         }
     }
     return 0;
 }


int class(tTNodePtr tree) {
    if (accept(CLASS_KEYWORD)) {
        tree->key = CLASS;
        tree->LPtr = malloc(sizeof(struct tTNode));
        tree->RPtr = NULL;
        token = getToken();
        ret = id(tree->LPtr);
        if (ret == 2) {
            return 2;
        }
        token = getToken();
        if (!accept(LEFT_CURLY_BRACKET)) {
            return 2; 
        }
        token = getToken();
        while (!accept(RIGHT_CURLY_BRACKET)) {
            tree->RPtr = malloc(sizeof(struct tTNode));
            ret = classitem(tree->RPtr);
            if (ret == 2) {
                return 2; 
            }
            tree = tree->RPtr;
            token = getToken();
        }
        return 0; 
    } else {
        return 2; 
    }
}


int classitem(tTNodePtr tree) {
    if (accept(STATIC_KEYWORD)) {
        tree->key = CLASS_ITEM;
        tTNodePtr temp = malloc(sizeof(struct tTNode));
        token = getToken();
        ret = declaration(temp);
        if (ret == 2) {
            return 2;
        }
        token = getToken();
        tree->LPtr = malloc(sizeof(struct tTNode));
        if (accept(LEFT_PARENTHESIS)) {
            ret = function(tree->LPtr,temp);            
        }
        if (accept(ASSIGNMENT_OPERATOR) || accept(SEMICOLON)) {
            ret = staticvar(tree->LPtr,temp);
        }
        return ret;
    } else {
        return 2;
    }
}


int function(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = FUNCTION;
    tree->RPtr = malloc(sizeof(struct tTNode));
    tree = tree->RPtr;
    tree->key = FUNCTION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    token = getToken(); 
    ret = arglist(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = getToken();
    if (!accept(LEFT_CURLY_BRACKET)) {
        return 2;    
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = stlist(tree->RPtr);
    if (ret == 2) {
        return 2;
    } 
    return 0;
}


int staticvar(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = STATIC_VAR;
    if (accept(ASSIGNMENT_OPERATOR)) {
        tree->RPtr = malloc(sizeof(struct tTNode));
        token = getToken();
        ret = safeExpression(tree->RPtr);
        if (ret == 2) {
            return 2;
        }
        if (!accept(SEMICOLON)) {
            return 2;
        }
    }
    return 0;
}


int declaration(tTNodePtr tree) {
    tree->key = DECLARATION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = type(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = id(tree->RPtr);
    if (ret == 2) {
        return 2;
    }
    return 0;
}


int arglist(tTNodePtr tree) {
    tree->key = ARG_LIST;
    if (accept(RIGHT_PARENTHESIS)) {
        return 0;
    }
    ret = argument(tree);
    if (ret == 2) {
        return 2;
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    while (1) {
        token = getToken();
        if (accept(COMMA_OPERATOR)) {
            token = getToken();
            tree = tree->RPtr; 
            ret = argument(tree);
            if (ret == 2) {
                return 2;
            }
            tree->key = ARG_LIST;
            tree->RPtr = malloc(sizeof(struct tTNode));      
        } else {
            break;
        }
    }
    if (accept(RIGHT_PARENTHESIS)) {
        if (tree != NULL) {
            free(tree->RPtr);
            tree->RPtr=NULL;
        } 
        return 0;
    } else {
        return 2;
    }
}


int argument(tTNodePtr tree) {
     tree->LPtr = malloc(sizeof(struct tTNode));
     ret = declaration(tree->LPtr);
     if (ret == 2) {
         return 2;
     }
     return 0;
}


int stlist(tTNodePtr tree) {
    tTNodePtr temp = NULL;
    tree->LPtr = NULL;
    while (1) {
        tree->key = ST_LIST;
    	if (accept(RIGHT_CURLY_BRACKET)) {
            if (temp != NULL) {
                free(temp->RPtr);
                temp->RPtr=NULL;
                
            }
            return 0;
    	}
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = localvar(tree->LPtr);
        if (ret == 2) {
            ret = statement(tree->LPtr);
            if (ret == 2) {
                return 2;
            }
        }
        tree->RPtr = malloc(sizeof(struct tTNode));
        temp = tree;
        tree = tree->RPtr;
    }
    return 0;
}

int localvar(tTNodePtr tree) {
    tree->key = LOCAL_VAR;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = getToken();
    if (accept(ASSIGNMENT_OPERATOR)) {
        token = getToken();
        tree->RPtr = malloc(sizeof(struct tTNode));
        ret = safeExpression(tree->RPtr);
        if (ret == 2) {
            return 2;
        }
    }
    if (!accept(SEMICOLON)) {
        return 2;
    }
    token = getToken();
    return 0;
}


int statement(tTNodePtr tree) {
    tree->key = STATEMENT;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->LPtr);
    if (ret != 2) {
        if (ret == 3) {
            return 2;
        } 
        token = getToken();
        return 0;
    }
    ret = condition(tree->LPtr);
    if (ret != 2) {
        if (ret == 3) {
            return 2;
        }
        token = getToken();
        return 0;
    }
    ret = cycle(tree->LPtr);
    if (ret != 2) {
        if (ret == 3) {
            return 2;
        }
        token = getToken(); 
        return 0;
    }
    ret = retval(tree->LPtr);
    if (ret != 2) {
        if (ret == 3) {
            return 2;
        }
        if (!accept(SEMICOLON)) {
            return 2;
        }
        token = getToken();
        return 0;
    }    
    tTNodePtr temp = malloc(sizeof(struct tTNode));
    ret = call(temp);
    if (ret != 2) {
        if (ret == 3) {
            return 2;
        }
        tree->LPtr = temp;
        token = getToken();
        if (!accept(SEMICOLON)) {
            return 2;
        }
        token = getToken();
        return 0; 
    }
    ret = assignment(tree->LPtr, temp);
    if (ret != 2) {
        if (ret == 3) {
            return 2;
        }
        if (!accept(SEMICOLON)) {
            return 2;
        }
        token = getToken();
        return 0;
    }
    free(tree->LPtr);
    tree->LPtr = NULL;
    return 2;
} 


int assignment(tTNodePtr tree, tTNodePtr temp) {   
    tree->key = ASSIGNMENT;
    tree->LPtr = temp->LPtr;
    free(temp);
    if (accept(ASSIGNMENT_OPERATOR)) {
        token = getToken(); 
        tree->RPtr = malloc(sizeof(struct tTNode));
        ret = safeExpression(tree->RPtr);
        if (ret == 2) {
            return 2;
        }
    } else {
        return 2;
    }
    return 0;
}


int block(tTNodePtr tree) {
    tree->key = BLOCK_LIST;
    if (accept(LEFT_CURLY_BRACKET)) {
        token = getToken();
        ret = blocklist(tree);
        if (ret == 2) {
            return 2;
        }
    } else {
        return 2;
    }
    return 0;
}


int blocklist(tTNodePtr tree) {
    tTNodePtr temp =NULL;
    while (1) {
        tree->key = BLOCK_LIST;
        if (accept(RIGHT_CURLY_BRACKET)) { 
            return 0;
        }
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = statement(tree->LPtr);
        if (ret == 2) {
            return 2;
        }
        tree->RPtr = malloc(sizeof(struct tTNode));
        tTNodePtr temp = tree; 
        tree = tree->RPtr;
    }
    if (temp != NULL) {
        free(temp->RPtr);
        temp->RPtr = NULL;
    } 
    return 0;
}


int condition(tTNodePtr tree) {
    tree->key = CONDITION;
    if (!accept(IF_KEYWORD)) {
        return 2;
    }
    token = getToken();
    if (!accept(LEFT_PARENTHESIS)) {
        return 3;
    }
    token = getToken();
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = comparison(tree->LPtr);
    if (ret == 2) {
        return 3;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return 3;
    }
    token = getToken();
    tree->RPtr = malloc(sizeof(struct tTNode));
    tree = tree->RPtr;
    tree->key = CONDITION;

    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->LPtr);
    if (ret == 2) {
        return 3;
    }

    token = getToken();
    if (!accept(ELSE_KEYWORD)) {
        return 3;
    }

    token = getToken();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->RPtr);
    if (ret == 2) {
        return 3;
    }
    return 0;
}


int cycle(tTNodePtr tree) {
    tree->key = CYCLE;
    if (!accept(WHILE_KEYWORD)) {
        return 2;
    }
    token = getToken();
    if (!accept(LEFT_PARENTHESIS)) {
        return 3;
    }
    token = getToken();
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = comparison(tree->LPtr);
    if (ret == 2) {
        return 3;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return 3;
    }
    token = getToken();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->RPtr);
    if (ret == 2) {
        return 3;
    }
    return 0;
}


int call(tTNodePtr tree) {
    tree->key = CALL;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = id(tree->LPtr);
    if (ret == 2) {
        return 3;
    }
    token = getToken();
    if (!accept(LEFT_PARENTHESIS)) {
        return 2;
    }
    token = getToken();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = parlist(tree->RPtr);
    if (ret == 2) {
        return 3;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return 3;
    }
    return 0;
}

 
int parlist(tTNodePtr tree) {
    tree->key = PAR_LIST;
    if (accept(RIGHT_PARENTHESIS)) {
        return 0;
    }
    ret = parameter(tree);
    if (ret == 2) {
        return 2;
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    while (1) {
        if (accept(COMMA_OPERATOR)) {
            token = getToken();
            tree = tree->RPtr; 
            tree->RPtr = malloc(sizeof(struct tTNode));
            ret = parameter(tree);
            if (ret == 2) {
                return 2;
            }
            tree->key = PAR_LIST; 
        } else {
            break;
        }
    }
    if (accept(RIGHT_PARENTHESIS)) {
        if (tree != NULL) {
            free(tree->RPtr);
            tree->RPtr=NULL;
        } 
        return 0;
    } else {
        return 2;
    }
}


int parameter(tTNodePtr tree) {
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = safeExpression(tree->LPtr);
    if (ret == 2) {
        return 2;
    }	
    return 0;
}


int retval(tTNodePtr tree) {
    if (!accept(RETURN_KEYWORD)) {
        return 2;
    }
    tree->key = RETURN;
    tree->LPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = safeExpression(tree->LPtr);
    if (ret == 2) {
        return 3;
    }
    return 0;
}


int safeExpression(tTNodePtr tree) {
    tTNodePtr higher = malloc(sizeof(struct tTNode));
    higher->key = EXPRESSION; 
    tTNodePtr temp = malloc(sizeof(struct tTNode));
    temp->key = EXPRESSION; 
    temp->LPtr = malloc(sizeof(struct tTNode));
    ret = expression(temp->LPtr);
    if (ret == 2) {
        return 2;
    }
    
    while (accept(ADDITION_OPERATOR) || accept(SUBTRACTION_OPERATOR)) {
        if (accept(ADDITION_OPERATOR)) {
            temp->literal = malloc(sizeof(char)*2);
            strcpy(temp->literal, "+");
        } else {
            temp->literal = malloc(sizeof(char)*2);
            strcpy(temp->literal, "-");
        } 
        temp->RPtr = malloc(sizeof(struct tTNode));
        token = getToken();
        ret = expression(temp->RPtr);
        if (ret == 2) {
            return 2;
        }
        higher->LPtr = temp;
        temp = higher;
        higher = malloc(sizeof(struct tTNode));
        higher->key = EXPRESSION;  
    }
    tree->LPtr = temp;
    tree->key = EXPRESSION;
    free(higher);
    return 0;
}


int expression(tTNodePtr tree) {
    tree->key = EXPRESSION;
    if (accept(LEFT_PARENTHESIS)) {
        token = getToken();
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = safeExpression(tree->LPtr);
        if (ret == 2) {
            return 2;
        } 
        if (!accept(RIGHT_PARENTHESIS)) {
            return 2;
        }
        token = getToken();  
    } else {
        ret = factor(tree);
        if (ret == 2) {
            return 2;
        }
    }

    while (accept(MULTIPLICATION_OPERATOR) || accept(DIVISION_OPERATOR)) {
        if (accept(MULTIPLICATION_OPERATOR)) {
            tree->literal = malloc(sizeof(char)*2);
            strcpy(tree->literal, "*");
        } else {
            tree->literal = malloc(sizeof(char)*2);
            strcpy(tree->literal, "/");
        }
        tree->RPtr = malloc(sizeof(struct tTNode));
        tree->key = TERM;
        tree = tree->RPtr;
        tree->key = EXPRESSION; 
        token = getToken();
        if (accept(LEFT_PARENTHESIS)) {
            tree->LPtr = malloc(sizeof(struct tTNode));
            ret = safeExpression(tree->LPtr);
            if (ret == 2) {
                return 2;
            } 
            if (!accept(RIGHT_PARENTHESIS)) {
                return 2;
            }
            token = getToken();  
        } else {
            ret = factor(tree);
            if (ret == 2) {
                return 2;
            }
        }
    }
    return 0;
}


int factor(tTNodePtr tree) {  
    if (accept(IDENTIFIER)) {
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = call(tree->LPtr);
        if (ret == 2 || ret == 3) {
            tTNodePtr temp = (tree->LPtr)->LPtr;
            //(tree->LPtr)->LPtr = NULL;
            //(tree->LPtr)->RPtr = NULL;
            free(tree->LPtr);
            tree->LPtr = temp;
            return 0;
        }
        token = getToken();
    } else {
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = literal(tree->LPtr);
        if (ret == 2) {
            return 2;
        }
        token = getToken();
    }
    return 0;
}


int literal(tTNodePtr tree) {
    tree->literal = malloc(sizeof(char)*strlen(token->data)+1);
    strcpy(tree->literal, token->data);
    if (accept(STRING_LITERAL)) {
        tree->key = STRING;
        return 0;
    }
    if (accept(INT_LITERAL)) {
        tree->key = INT;
        return 0;
    }

    if (accept(DOUBLE_LITERAL)) {
        tree->key = DOUBLE;
        return 0;
    }
    return 2;
}


int type(tTNodePtr tree) {
    tree->LPtr = NULL;
    tree->RPtr = NULL;
    if (accept(INT_DATA_TYPE)) {
        tree->key = INT_DATA;
        return 0;
    }
    if (accept(STRING_DATA_TYPE)) {
        tree->key = STRING_DATA;
        return 0;
    }
    if (accept(DOUBLE_DATA_TYPE)) {
        tree->key = DOUBLE_DATA;
        return 0;
    }

    if (accept(VOID_KEYWORD)) {
        tree->key = VOID_DATA;
        return 0;
    }

    return 2;
}


int id(tTNodePtr tree) {
    if (!accept(IDENTIFIER)) {
        return 2;
    }
    tree->key = ID;
    tree->LPtr = NULL;
    tree->RPtr = NULL;
    tree->literal = malloc(sizeof(char)*strlen(token->data)+1);
    strcpy(tree->literal, token->data);
    return 0;
}


int comparison(tTNodePtr tree) {
    tree->key = COMPARISON;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = safeExpression(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    if (accept(RIGHT_PARENTHESIS)) {
        return 0;
    }
    
    switch (token->type) {
        case EQUAL_TO_OPERATOR: tree->literal = malloc(sizeof(char)*3);
                strcpy(tree->literal, "=="); break;
        case NOT_EQUAL_TO_OPERATOR: tree->literal = malloc(sizeof(char)*3);
                strcpy(tree->literal, "!="); break;
        case GREATER_THAN_OR_EQUAL_TO_OPERATOR: tree->literal = malloc(sizeof(char)*3);
                strcpy(tree->literal, ">="); break;
        case LESS_THAN_OR_EQUAL_TO_OPERATOR: tree->literal = malloc(sizeof(char)*3);
                strcpy(tree->literal, "<="); break;
        case GREATER_THAN_OPERATOR: tree->literal = malloc(sizeof(char)*2);
                strcpy(tree->literal, ">"); break;
        case LESS_THAN_OPERATOR: tree->literal = malloc(sizeof(char)*2);
                strcpy(tree->literal, "<"); break;
        default: return 2;
    }

    token = getToken();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = safeExpression(tree->RPtr);
    if (ret == 2) {
        return 2;
    }
    return 0; 
}
