#include "parser.h"

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
int assignment(tTNodePtr tree);
int block(tTNodePtr tree);
int blocklist(tTNodePtr tree);
int condition(tTNodePtr tree);
int cycle(tTNodePtr tree);
int call(tTNodePtr tree);
int parlist(tTNodePtr tree);
int parameter(tTNodePtr tree);
int retval(tTNodePtr tree);
int expression(tTNodePtr tree);
int parenthesis(tTNodePtr tree);
int term(tTNodePtr tree);
int factor(tTNodePtr tree);    
int literal(tTNodePtr tree);
int id(tTNodePtr tree);
int type(tTNodePtr tree);
int comparison(tTNodePtr tree);

 inline void treeInit(tTNodePtr tree) {
     tree = NULL;
 }


int accept(type_t sym) {
    if (token != NULL) {
     	return (int) (token->type == sym);
    } else {
        return 0;
    }
 }


 int file(tTNodePtr tree) {
     return classList(tree);
 }


 int classList(tTNodePtr tree) {
     token = get_token();
     if (token == NULL) {
         return 2; //file empty
     }

     tree = malloc(sizeof(struct tTNode));
     tree->key = CLASS_LIST;
     tree->LPtr = malloc(sizeof(struct tTNode));
     tree->RPtr = malloc(sizeof(struct tTNode));

     ret = class(tree->LPtr);
     if (ret == 2) {
         return 2;
     } 
     
     tree = tree->RPtr;
     while (1) {
         token = get_token();
         if (token == NULL) {
            free(tree->RPtr);
            return 0; //end of file
         }

         tree->key = CLASS_LIST;
         tree->LPtr = malloc(sizeof(struct tTNode));

         ret = class(tree->LPtr);
         if (ret == 2) {
            return 2; //error in class
         }

         tree->RPtr = malloc(sizeof(struct tTNode));
         tree = tree->RPtr;
     }
 }


int class(tTNodePtr tree) {
    if (accept(KEYWORD_CLASS)) {
        tree->key = CLASS;
        tree->LPtr = malloc(sizeof(struct tTNode));
        tree->RPtr = NULL;
        token = get_token();
        ret = id(tree->LPtr);
        if (ret == 2) {
            return 2; //non valid id
        }
        token = get_token();
        if (!accept(LEFT_CURLY_BRACKET)) {
            return 2; //no curly bracket
        }
        token = get_token();
        while (!accept(RIGHT_CURLY_BRACKET)) {
            tree->RPtr = malloc(sizeof(struct tTNode));
            ret = classitem(tree->RPtr);
            if (ret == 2) {
                return 2; //error in classitem
            }
            tree->RPtr = malloc(sizeof(struct tTNode));
            tree = tree->RPtr;
            token = get_token();
        }
        return 0; //everything works
    } else {
        return 2; //not a class
    }
}


int classitem(tTNodePtr tree) {
    if (accept(KEYWORD_STATIC)) {
        tree->key = CLASS_ITEM;
        tTNodePtr temp = malloc(sizeof(struct tTNode));
        token = get_token();
        ret = declaration(temp);
        if (ret == 2) {
            return 2;
        }
        token = get_token();
        tree->LPtr = malloc(sizeof(struct tTNode));
        if (accept(LEFT_PARENTHESIS)) {
            ret = function(tree->LPtr,temp);            
        } else if (accept(EQUAL_TO_OPERATOR) || accept(SEMICOLON)) {
            ret = staticvar(tree->LPtr,temp);
        } else {
            return 2; //neither function nor static var
        }
        return 0; //all is right
    } else {
        return 2;
    }
}


int function(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = FUNCTION;
    token = get_token();
    tree->RPtr = malloc(sizeof(struct tTNode));
    tree = tree->RPtr;
    tree->key = FUNCTION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = arglist(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return 2;    
    }
    token = get_token();
    if (!accept(LEFT_CURLY_BRACKET)) {
        return 2;    
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    token = get_token();
    ret = stlist(tree->RPtr);
    if (ret == 2) {
        return 2;
    } 
    if (accept(RIGHT_CURLY_BRACKET)) {
        return 2;    
    }
    return 0;
}


int staticvar(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = STATIC_VAR;
    if (accept(EQUAL_TO_OPERATOR)) {
        tree->RPtr = malloc(sizeof(struct tTNode));
        token = get_token();
        ret = assignment(tree->RPtr);
        if (ret == 2) {
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
    token = get_token();
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
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = argument(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    while (1) {
        token = get_token();
        if (accept(COMMA_OPERATOR)) {
            token = get_token();
            tree->RPtr = malloc(sizeof(struct tTNode));
            ret = argument(tree->RPtr);
            if (ret == 2) {
                return 2;
            }
            tree = tree->RPtr;
        } else {
            break;
        }
    }
    if (token->type == RIGHT_PARENTHESIS) {
        return 0;
    } else {
        return 2;
    }
}


int argument(tTNodePtr tree) {
     tree->LPtr = malloc(sizeof(struct tTNode));
     tree->key = ARG;
     ret = declaration(tree->LPtr);
     if (ret == 2) {
         return 2;
     }
     return 0;
}


int stlist(tTNodePtr tree) {
    if (accept(RIGHT_CURLY_BRACKET)) {
        return 0;
    }
    while (1) {
        tree->key = ST_LIST;
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = localvar(tree->LPtr);
        if (ret == 2) {
            ret = statement(tree->LPtr);
            if (ret == 2) {
                return 2;
            }
        }
        tree->RPtr = malloc(sizeof(struct tTNode));
        tree = tree->RPtr;
    }
    free(tree->RPtr);
    return 0;
}

int localvar(tTNodePtr tree) {
    tree->key = LOCAL_VAR;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = get_token();
    if (accept(EQUAL_TO_OPERATOR)) {
        token = get_token();
        tree->RPtr = malloc(sizeof(struct tTNode));
        ret = assignment(tree->RPtr);
        if (ret == 2) {
            return 2;
        }
    }
    if (token->type != SEMICOLON) {
        return 2;
    }
    return 0;
}


int statement(tTNodePtr tree) {
    tree->key = STATEMENT;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = assignment(tree->LPtr);
    if (ret != 2) {
        token = get_token();
        return accept(SEMICOLON);
    }
    ret = block(tree->LPtr);
    if (ret != 2) {
        return 0;
    }
    ret = condition(tree->LPtr);
    if (ret != 2) {
        return 0;
    }
    ret = cycle(tree->LPtr);
    if (ret != 2) {
        return 0;
    }
    ret = call(tree->LPtr);
    if (ret != 2) {
        token = get_token();
        return accept(SEMICOLON);
    }
    free((tree->LPtr)->LPtr);
    ret = retval(tree->LPtr);
    if (ret != 2) {
        token = get_token();
        return accept(SEMICOLON);
    }

    return 2;
} 


int assignment(tTNodePtr tree) {
    tree->key = ASSIGNMENT;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = get_token();
    if (accept(ASSIGNMENT_OPERATOR)) {
        tree->RPtr = malloc(sizeof(struct tTNode));
        ret = assignment(tree->RPtr);
        if (ret == 2) {
            return 2;
        }
    }
    return 0;
}


int block(tTNodePtr tree) {
    if (accept(LEFT_CURLY_BRACKET)) {
        tree->key = BLOCK;
        tree->LPtr = malloc(sizeof(struct tTNode));
        token = get_token();
        ret = blocklist(tree->LPtr);
        if (ret = 2) {
            return 2;
        }
    } else {
        return 2;
    }
    if (accept(RIGHT_CURLY_BRACKET)) {
        return 0;
    } else {
        return 2;
    }
}


int blocklist(tTNodePtr tree) {
    if (accept(RIGHT_CURLY_BRACKET)) {
        return 0;
    }
    while (1) {
        tree->key = BLOCK_LIST;
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = statement(tree->LPtr);
        if (ret == 2) {
            return 2;
        }
        tree->RPtr = malloc(sizeof(struct tTNode));
        tree = tree->RPtr;
    }
    free(tree->RPtr);
    return 0;
}


int condition(tTNodePtr tree) {
    tree->key = CONDITION;
    if (!accept(KEYWORD_IF)) {
        return 2;
    }
    token = get_token();
    if (!accept(LEFT_PARENTHESIS)) {
        return 2;
    }
    token = get_token();
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = comparison(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = get_token();
    if (!accept(RIGHT_PARENTHESIS)) {
        return 2;
    }
    token = get_token();
    tree->RPtr = malloc(sizeof(struct tTNode));
    tree = tree->RPtr;
    tree->key = CONDITION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = get_token();
    if (!accept(KEYWORD_ELSE)) {
        return 2;
    }
    token = get_token();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->RPtr);
    if (ret == 2) {
        return 2;
    }
    return 0;
}


int cycle(tTNodePtr tree) {
    tree->key = CYCLE;
    if (!accept(KEYWORD_WHILE)) {
        return 2;
    }
    token = get_token();
    if (!accept(LEFT_PARENTHESIS)) {
        return 2;
    }
    token = get_token();
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = comparison(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    token = get_token();
    if (!accept(RIGHT_PARENTHESIS)) {
        return 2;
    }
    token = get_token();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->RPtr);
    if (ret == 2) {
        return 2;
    }
    return 0;
}


int call(tTNodePtr tree) {
    tree->key = CALL;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = id(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    if (!accept(LEFT_PARENTHESIS)) {
        return 2;
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = parlist(tree->RPtr);
    if (ret = 2) {
        return 2;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return 2;
    }
    return 0;
}


int parlist(tTNodePtr tree) {
    tree->key = PAR_LIST;
    if (token->type == RIGHT_PARENTHESIS) {
        return 0;
    }
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = parameter(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    while (1) {
        token = get_token();
        if (token->type == COMMA_OPERATOR) {
            token = get_token();
            tree->RPtr = malloc(sizeof(struct tTNode));
            ret = parameter(tree->RPtr);
            if (ret == 2) {
                return 2;
            }
            tree = tree->RPtr;
        } else {
            break;
        }
    }
    if (token->type == RIGHT_PARENTHESIS) {
        return 0;
    } else {
        return 2;
    }
}


int parameter(tTNodePtr tree) {
    tree->key = PAR;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == 2) {
        ret = expression(tree->LPtr);
        if (ret == 2) {
            return 2;
        }
        return 0;
    }
    return 0;
}


int retval(tTNodePtr tree) {
    if (!accept(KEYWORD_RETURN)) {
        return 0;
    }
    tree->key = RETURN;
    tree->LPtr = malloc(sizeof(struct tTNode));
    token = get_token();
    ret = expression(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    return 0;
}


int expression(tTNodePtr tree) {
    tree->key = EXPRESSION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    token = get_token();
    if (accept(LEFT_PARENTHESIS)) {
        ret = parenthesis(tree->LPtr);
    } else {
        ret = term(tree->LPtr);
    }
    if (ret == 2) {
        return 2;
    }

    tree->key = EXPRESSION;

    token = get_token();
    while (accept(ADDITION_OPERATOR) || accept(SUBTRACTION_OPERATOR)) {
        token = get_token();
        tree->RPtr = malloc(sizeof(struct tTNode));        
        if (accept(LEFT_PARENTHESIS)) {
            ret = parenthesis(tree->RPtr);
        } else {
            ret = term(tree->RPtr);
        }
        if (ret == 2) {
            return 2;
        }
        token = get_token();
        tree = tree->RPtr;
        tree->key = EXPRESSION;
    }
    return 0;
}


int parenthesis(tTNodePtr tree) {
    ret = expression(tree);
    if (ret == 2) {
        return 2;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return 2;
    }
    return 0;
}


int term(tTNodePtr tree) {
    tree->key = TERM;
    tree->LPtr = malloc(sizeof(struct tTNode));  
    token = get_token();  
    if (accept(LEFT_PARENTHESIS)) {
        ret = parenthesis(tree->LPtr);
    } else {
        ret = factor(tree->LPtr);
    }
    if (ret == 2) {
        return 2;
    }

    token = get_token();
    while (accept(MULTIPLICATION_OPERATOR) || accept(DIVISION_OPERATOR)) {
        token = get_token();
        tree->RPtr = malloc(sizeof(struct tTNode));
        if (accept(LEFT_PARENTHESIS)) {
            ret = parenthesis(tree->RPtr);
        } else {
            ret = factor(tree->RPtr);
        }
        if (ret == 2) {
            return 2;
        }
        token = get_token();
        tree = tree->RPtr;
        tree->key = TERM;
    }
    return 0;
}


int factor(tTNodePtr tree) {
    tree->key = FACTOR;
    tTNodePtr temp = malloc(sizeof(struct tTNode));
    token = get_token();
    ret = call(temp);
    if (ret == 2) {
        if (temp->LPtr != NULL) {
            tree->LPtr = temp->LPtr;
            return 0;
        }
    }
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = literal(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    return 0;
}


int literal(tTNodePtr tree) {
    tree->literal = malloc(sizeof(char)*strlen(token->data)+1);
    strcpy(tree->literal, token->data);
    if (accept(STRING_LITERAL)) {
        tree->key = STRING;
        tree->LPtr = malloc(sizeof(struct tTNode));
    }
    if (accept(INT_LITERAL)) {
        tree->key = INT;
        tree->LPtr = malloc(sizeof(struct tTNode));
        tree->literal = malloc(sizeof(char)*strlen(token->data)+1);
        strcpy(tree->literal, token->data);
    }

    if (accept(DOUBLE_LITERAL)) {
        tree->key = INT;
        tree->LPtr = malloc(sizeof(struct tTNode));
        tree->literal = malloc(sizeof(char)*strlen(token->data)+1);
        strcpy(tree->literal, token->data);
    }
    return 2;
}


int type(tTNodePtr tree) {
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

    if (accept(VOID_DATA_TYPE)) {
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
    tree->literal = malloc(sizeof(char)*strlen(token->data)+1);
    strcpy(tree->literal, token->data);
    return 0;
}


int comparison(tTNodePtr tree) {
    tree->key = COMPARISON;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = expression(tree->LPtr);
    if (ret == 2) {
        return 2;
    }
    if (!accept(GREATER_THAN_OPERATOR) && !accept(LESS_THAN_OPERATOR) && !accept(GREATER_THAN_OR_EQUAL_TO_OPERATOR)) {
        if (!accept(LESS_THAN_OR_EQUAL_TO_OPERATOR) && !accept(EQUAL_TO_OPERATOR) && !accept(NOT_EQUAL_TO_OPERATOR)) {
            return 2;       
        }
    }    
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = expression(tree->RPtr);
    if (ret == 2) {
        return 2;
    }
    return 0; 
}
