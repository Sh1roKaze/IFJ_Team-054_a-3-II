#include "parser.h"

 inline void treeInit(tTNodePtr tree) {
     tree = NULL;
 }


 char *mallocString(char *s) {
     char *temp = malloc(sizeof(char)*strlen(s));
     strcpy(temp, s);
     return temp;
 }


 int file(tTNodePtr tree) {
     treeInit(tree);
     return classList(tree);
 }


 int classList(tTNodePtr tree) {
     token = getToken();
     if (token == NULL) {
         return ERROR;
     }

     tree = malloc(sizeof(struct tTNode));
     tree->key = mallocString("classlist");
     tree->LPtr = malloc(sizeof(struct tTNode));
     tree->MPtr = NULL;

     ret = class(tree->LPtr);
     if (ret == ERROR_CODE) {
         return ERROR;
     }

     tTNodePtr temp = tree->MPtr;
     while (1) {
         token = getToken();
         if (token == NULL) {
            return 0;
         }
         temp = malloc(sizeof(struct tTNode));
         temp->key = mallocString("classlist");
         temp->LPtr = malloc(sizeof(struct tTNode));
         temp->MPtr = NULL;

         ret = class(temp->LPtr);
         if (ret == ERROR_CODE) {
            return ERROR;
         }
         temp = temp->MPtr;
     }
 }


int class(tTNodePtr tree) {
    if (token->type == class) {
        tree->key = mallocString("class");
        tree->LPtr = malloc(sizeof(struct tTNode));
        tree->MPtr = NULL;
        ret = id(tree->LPtr);
        if (ret == ERROR_CODE) {
            return ERROR;
        }
        token = getToken();
        if (token->type != "{") {
            return ERROR;
        }
        token = getToken();
        while (token->type != "}") {
            tree->MPtr = malloc(sizeof(struct tTNode));
            ret = classitem(tree->MPtr);
            if (ret == ERROR_CODE) {
                return ERROR;
            }
            tree = tree->MPtr;
            token = getToken();
        }
        return 0;
    } else {
        return ERROR;
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
            return ERROR;
        }
    } else {
        return ERROR;
    }
    if (ret == ERROR_CODE) {
        return ERROR;
    } else {
        return 0;
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
        return ERROR_CODE;    
    }
    token = getToken();
    if (token->type != "{") {
        return ERROR_CODE;    
    }
    tree->RPtr = malloc(sizeof(struct tTNode));
    token = getToken();
    ret = stlist(tree->RPtr);
    if (ret == ERROR_CODE) {
        return ERROR;
    } 
    if (token->type != "}") {
        return ERROR_CODE;    
    }
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
}


int declaration(tTNodePtr tree) {
    tree->key = mallocString("declaration");
    tree->LPtr = malloc(sizeof(struct tTNode));
    token = getToken();
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
}


int arglist(tTNodePtr tree) {
    tree->key = mallocString("arglist");
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
}


int argument(tTNodePtr tree) {
     tree->LPtr = malloc(sizeof|(struct tTNode));
     tree->key = mallocString("arg");
     ret = declaration(tree->LPtr);
     if (ret == ERROR_CODE) {
         return ERROR;
     }
}