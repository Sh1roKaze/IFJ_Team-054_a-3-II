#include "parser.h"

 inline void treeInit(tTNodePtr tree) {
     tree = NULL;
 }


 inline int accept(type_t sym) {
     return (token->type == sym);
 }


 int file(tTNodePtr tree) {
     return classList(tree);
 }


 int classList(tTNodePtr tree) {
     token = getToken();
     if (token == NULL) {
         return ERROR; //file empty
     }

     tree = malloc(sizeof(struct tTNode));
     tree->key = CLASS_LIST;
     tree->LPtr = malloc(sizeof(struct tTNode));
     tree->MPtr = NULL;

     ret = class(tree->LPtr);
     if (ret == ERROR_CODE) {
         return ERROR; //at least one class must exist
     } 
     
     tree = tree->MPtr;
     while (1) {
         token = getToken();
         if (token == NULL) {
            free(tree->MPtr);
            return 0; //end of file
         }

         tree->key = CLASS_LIST;
         tree->LPtr = malloc(sizeof(struct tTNode));

         ret = class(temp->LPtr);
         if (ret == ERROR_CODE) {
            return ERROR; //error in class
         }

         tree->MPtr = malloc(sizeof(struct tTNode));
         tree = tree->MPtr;
     }
 }


int class(tTNodePtr tree) {
    if (token->type == KEYWORD_CLASS) {
        tree->key = CLASS;
        tree->LPtr = malloc(sizeof(struct tTNode));
        tree->MPtr = NULL;
        ret = id(tree->LPtr);
        if (ret == ERROR_CODE) {
            return ERROR; //non valid id
        }
        token = getToken();
        if (accept(LEFT_CURLY_BRACKET)) {
            return ERROR; //no curly bracket
        }
        token = getToken();
        while (token->type != "}") {
            tree->MPtr = malloc(sizeof(struct tTNode));
            ret = classitem(tree->MPtr);
            if (ret == ERROR_CODE) {
                return ERROR; //error in classitem
            }
            tree->MPtr = malloc(sizeof(struct tTNode));
            tree = tree->MPtr;
            token = getToken();
        }
        return 0; //everything works
    } else {
        return ERROR; //not a class
    }
}


int classitem(tTNodePtr tree) {
    if (token->type == KEYWORD_STATIC) {
        tree->key = CLASS_ITEM;
        tTNodePtr temp = malloc(sizeof(struct tTNode));
        token = getToken();
        ret = declaration(temp);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
        token = getToken();
        tree->LPtr = malloc(sizeof(struct tTNode));
        if (token->type == "(") {
            ret = function(tree->LPtr,temp);            
        } else if (token->type == "=" || token->type == ";") {
            ret = staticvar(tree->LPtr,temp);
        } else {
            return ERROR; //neither function nor static var
        }
        return 0; //all is right
        }
    } else {
        return ERROR;
    }
}


int function(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = FUNCTION;
    token = getToken();
    tree->MPtr = malloc(sizeof(struct tTNode));
    ret = arglist(tree->MPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    if (token->type != RIGHT_PARENTHESIS) {
        return ERROR;    
    }
    token = getToken();
    if (token->type != LEFT_CURLY_BRACKET) {
        return ERROR;    
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = stlist(tree->RPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } 
    if (token->type != RIGHT_CURLY_BRACKET) {
        return ERROR;    
    }
    return 0;
}


int staticvar(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = STATIC_VAR;
    if (token->type == EQUAL_TO_OPERATOR) {
        tree->MPtr = malloc(sizeof(struct tTNode));
        token = getToken();
        ret = assigment(tree->MPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
    }
    return 0;
}


int declaration(tTNodePtr tree) {
    tree->key = DECLARATION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = type(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    tree->MPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = id(tree->MPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    return 0;
}


int arglist(tTNodePtr tree) {
    tree->key = ARG_LIST;
    if (token->type == RIGHT_PARENTHESIS) {
        return 0;
    }
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = argument(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    while (1) {
        token = getToken();
        if (token->type == COMMA_OPERATOR) {
            token = getToken();
            tree->MPtr = malloc(sizeof(struct tTNode));
            ret = argument(tree->MPtr);
            if (ret == ERROR_CODE) {
                return ERROR;
            }
            tree = tree->MPtr;
        } else {
            break;
        }
    }
    if (token->type == RIGHT_PARENTHESIS) {
        return 0;
    } else {
        return ERROR;
    }
}


int argument(tTNodePtr tree) {
     tree->LPtr = malloc(sizeof(struct tTNode));
     tree->key = ARG;
     ret = declaration(tree->LPtr);
     if (ret == ERROR_CODE) {
         return ERROR;
     }
     return 0;
}


int stlist(tTNodePtr tree) {
    if (token->type == RIGHT_CURLY_BRACKET) {
        return 0;
    }
    while (1) {
        tree->key = ST_LIST;
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = localvar(tree->LPtr);
        if (ret == ERROR_CODE) {
            ret = statement(tree->LPtr);
            if (ret == ERROR_CODE) {
                return ERROR;
            }
        }
        tree->MPtr = malloc(sizeof(struct tTNode));
        tree = tree->MPtr
    }
    free(tree->MPtr);
    return 0;
}

int localvar(tTNodePtr tree) {
    tree->key = LOCAL_VAR;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    token = getToken();
    if (token->type == EQUAL_TO_OPERATOR) {
        token = getToken();
        tree->MPtr = malloc(sizeof(struct tTNode));
        ret = assignment(tree->MPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
    }
    if (token->type != SEMICOLON) {
        return ERROR;
    }
    return 0;
}


int statement(tTNodePtr tree) {
    tree->key = STATEMENT;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = assignment(tree->LPtr);
    if (ret != ERROR_CODE) {
        token = getToken();
        return accept(SEMICOLON);
    }
    ret = block(tree->LPtr);
    if (ret != ERROR_CODE) {
        return 0;
    }
    ret = condition(tree->LPtr);
    if (ret != ERROR_CODE) {
        return 0;
    }
    ret = cycle(tree->LPtr);
    if (ret != ERROR_CODE) {
        return 0;
    }
    ret = call(tree->LPtr);
    if (ret != ERROR_CODE) {
        token = getToken();
        return accept(SEMICOLON);
    }
    free((tree->LPtr)->LPtr);
    ret = retval(tree->LPtr);
    if (ret != ERROR_CODE) {
        token = getToken();
        return accept(SEMICOLON);
    }

    return ERROR;
} 


int assignment(tTNodePtr tree) {
    tree->key = ASSIGNMENT;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    token = getToken();
    if (accept(ASSIGNMENT_OPERATOR)) {
        tree->MPtr = malloc(sizeof(struct tTNode));
        ret = assignment(tree->MPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
    }
    return 0;
}


int block(tTNodePtr tree) {
    if (accept(LEFT_CURLY_BRACKET)) {
        tree->key = BLOCK;
        tree->LPtr = malloc(sizeof(struct tTNode));
        token = getToken();
        ret = blocklist(tree->LPtr);
        if (ret = ERROR_CODE) {
            return ERROR;
        }
    } else {
        return ERROR;
    }
    if (accept(RIGHT_CURLY_BRACKET)) {
        return 0;
    } else {
        return ERROR;
    }
}


int bloklist(tTNodePtr tree) {
    if (accept(RIGHT_CURLY_BRACKET) {
        return 0;
    }
    while (1) {
        tree->key = BLOCKLIST;
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = statement(tree->LPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
        tree->MPtr = malloc(sizeof(struct tTNode));
        tree = tree->MPtr
    }
    free(tree->MPtr);
    return 0;
}


int condition(tTNodePtr tree) {
    tree->key = CONDITION;
    if (!accept(KEYWORD_IF)) {
        return ERROR;
    }
    token = getToken();
    if (!accept(LEFT_PARENTHESIS)) {
        return ERROR;
    }
    token = getToken();
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = comparison(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    token = getToken();
    if (!accept(RIGHT_PARENTHESIS)) {
        return ERROR;
    }
    token = getToken();
    tree->MPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->MPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    token = getToken();
    if (!accept(KEYWORD_ELSE)) {
        return ERROR;
    }
    token = getToken();
    tree->RPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->RPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    return 0;
}


int cycle(tTNodePtr tree) {
    tree->key = CYCLE;
    if (!accept(KEYWORD_WHILE)) {
        return ERROR;
    }
    token = getToken();
    if (!accept(LEFT_PARENTHESIS)) {
        return ERROR;
    }
    token = getToken();
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = comparison(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    token = getToken();
    if (!accept(RIGHT_PARENTHESIS)) {
        return ERROR;
    }
    token = getToken();
    tree->MPtr = malloc(sizeof(struct tTNode));
    ret = block(tree->MPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    return 0;
}


int call(tTNodePtr tree) {
    tree->key = CALL;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = id(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    if (!accept(LEFT_PARENTHESIS)) {
        return ERROR;
    }
    tree->MPtr = malloc(sizeof(struct tTNode));
    ret = parlist(tree->MPtr);
    if (ret = ERROR_CODE) {
        return ERROR;
    }
    if (!accept(RIGHT_PARENTHESIS)) {
        return ERROR;
    }
    return 0;
}

int bloklist(tTNodePtr tree) {
    if (accept(RIGHT_PARENTHESIS) {
        return 0;
    }
    while (1) {
        tree->key = PAR_LIST;
        tree->LPtr = malloc(sizeof(struct tTNode));
        ret = parameter(tree->LPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
        tree->MPtr = malloc(sizeof(struct tTNode));
        tree = tree->MPtr
    }
    free(tree->MPtr);
    return 0;
}


int parameter(tTNodePtr tree) {
    tree->key = PARAMETER;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == ERROR_CODE) {
        ret = expression(tree->LPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
        return 0;
    }
    return 0;
}


int retval(tTNodePtr tree) {
    if (!accept(KEYWORD_RETURN)) {
        return 0;
    }
    tree->key = KEYWORD_RETURN;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = expression(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    return 0;
}


int expression(tTNodePtr tree) {
    tree->key = EXPRESSION;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = term(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    while (1) {
        token = getToken();
        if (token->type == ADDITION_OPERATOR || token->type == SUBTRACTION_OPERATOR) {
            token = getToken();
            tree->MPtr = malloc(sizeof(struct tTNode));
            ret = term(tree->MPtr);
            if (ret == ERROR_CODE) {
                return ERROR;
            }
            tree = tree->MPtr;
        } else {
            break;
        }
    }
    return 0;
}


int term(tTNodePtr tree) { 
    tree->key = TERM;
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = factor(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    while (1) {
        token = getToken();
        if (token->type == MULTIPLICATION_OPERATOR || token->type == DIVISION_OPERATOR) {
            token = getToken();
            tree->MPtr = malloc(sizeof(struct tTNode));
            ret = factor(tree->MPtr);
            if (ret == ERROR_CODE) {
                return ERROR;
            }
            tree = tree->MPtr;
        } else {
            break;
        }
    }
    return 0;
}


int factor(tTNodePtr tree) {
    tree->key = FACTOR;
    tTNodePtr temp = malloc(sizeof(struct tTNode));
    token = get_token();
    ret = call(temp);
    if (ret == ERROR) {
        if (temp->LPtr != NULL) {
            tree->LPtr = temp->LPtr;
            return 0;
        }
    }
    tTNodePtr tree->LPtr = malloc(sizeof(struct tTNode));
    ret = literal(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    return 0;
}


int literal(tTNodePtr tree) {
    if (accept(STRING_LITERAL)) {
        tree->key = STRING_LITERAL;
        tree->LPtr = malloc(sizeof(struct tTNode));
        //actual string
    }
    if (accept(NUMBER)) {
        tree->key = NUMBER;
        tree->LPtr = malloc(sizeof(struct tTNode));
        //actual number
    }
    return ERROR;
}


int type(tTNodePtr tree) {
    if (accept(INT_DATA_TYPE)) {
        tree->key = INT_DATA_TYPE;
        return 0;
    }
    if (accept(STRING_DATA_TYPE)) {
        tree->key = STRING_DATA_TYPE;
        return 0;
    }
    if (accept(DOUBLE_DATA_TYPE)) {
        tree->key = DOUBLE_DATA_TYPE;
        return 0;
    }
}


int id(tTNodePtr tree) {
    if (!accept(IDENTIFIER)) {
        return ERROR;
    }
    tree->key = IDENTIFIER;
    //actual identifier
    return 0;
}


int comparison(tTNodePtr tree) {
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = expression(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    if (!accept(GREATER_THAN_OPERATOR)) {
        if (!accept(LESS_THAN_OPERATOR)) {
            if (!accept(GREATER_THAN_OR_EQUAL_TO_OPERATOR)) {
                if (!accept(LESS_THAN_OR_EQUAL_TO_OPERATOR)) {
                    if (!accept(EQUAL_TO_OPERATOR)) {
                        if (!accept(NOT_EQUAL_TO_OPERATOR)) {
                            return ERROR;       
                        }
                    }
                }    
            }
        }
    }
    tree->MPtr = malloc(sizeof(struct tTNode));
    ret = expression(tree->MPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } 
}