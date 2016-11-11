#include "parser.h"

 inline void treeInit(tTNodePtr tree) {
     tree = NULL;
 }


 char *mallocString(char *s) {
     char *temp = malloc(sizeof(char)*strlen(s));
     strcpy(temp, s);
     return temp;
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
     tree->key = mallocString("classlist");
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

         tree->key = mallocString("classlist");
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
    if (token->type == class) {
        tree->key = mallocString("class");
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
    if (token->type == "static") {
        tree->key = mallocString("classitem");
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
    tree->key = mallocString("function");
    token = getToken();
    tree->MPtr = malloc(sizeof(struct tTNode));
    ret = arglist(tree->MPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    if (token->type != ")") {
        return ERROR;    
    }
    token = getToken();
    if (token->type != "{") {
        return ERROR;    
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = stlist(tree->RPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } 
    if (token->type != "}") {
        return ERROR;    
    }
    return 0;
}


int staticvar(tTNodePtr tree, tTNodePtr temp) {
    tree->LPtr = temp;
    tree->key = mallocString("staticvar")
    if (token->type == "=") {
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
    tree->key = mallocString("declaration");
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
    tree->key = mallocString("arglist");
    if (token->type == ")") {
        return 0;
    }
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = argument(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    while (1) {
        token = getToken();
        if (token->type == ",") {
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
    return 0;
}


int argument(tTNodePtr tree) {
     tree->LPtr = malloc(sizeof(struct tTNode));
     tree->key = mallocString("arg");
     ret = declaration(tree->LPtr);
     if (ret == ERROR_CODE) {
         return ERROR;
     }
     return 0;
}


int stlist(tTNodePtr tree) {
    if (token->type == "}") {
        return 0;
    }
    while (1) {
        tree->key = mallocString("stlist");
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
    tree->key = mallocString("localvar");
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    }
    token = getToken();
    if (token->type == "=") {
        token = getToken();
        tree->MPtr = malloc(sizeof(struct tTNode));
        ret = assignment(tree->MPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
    }
    if (token->type != ";") {
        return ERROR;
    }
    return 0;
}


int statement(tTNodePtr tree) {
    tree->key = mallocString("statement");
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = assignment(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        token = getToken();
        return accept(";");
    }
    ret = block(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        return 0;
    }
    ret = condition(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        return 0;
    }
    ret = cycletree->LPtr();
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        return 0;
    }
    ret = call(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        token = getToken();
        return accept(";");
    }
    ret = retval(tree->LPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        token = getToken();
        return accept(";");
    }

    return ERROR;
} 


int assignment(tTNodePtr tree) {
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = declaration(tree->LPtr);
    if (ret == ERROR_CODE) {
        return 0;
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
        token = getToken();
        ret = blocklist();
        if (ret = ERROR_CODE) {
            return ERROR;
        }
    } else {
        return 0;
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
        tree->key = mallocString("blocklist");
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
    tree->key = mallocString("condition");
    if (!accept(KEYWORD_IF)) {
        return 0;
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
    tree->key = mallocString("cycle");
    if (!accept(KEYWORD_WHILE)) {
        return 0;
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
    tree->key = mallocString("call");
    tree->LPtr = malloc(sizeof(struct tTNode));
    ret = id(tree->LPtr);
    if (ret == ERROR_CODE) {
        free(tree->LPtr);
        return 0;
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
        tree->key = mallocString("parlist");
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
    tree->key = mallocString("paramater");
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