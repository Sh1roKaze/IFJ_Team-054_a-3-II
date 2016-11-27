#include "interpret.h"

/**
* Vytiskne upozornění na to, že došlo k chybě.
*/
void Error() {

    printf ("*ERROR* The program has performed an illegal operation.\n");
}

/**
* Provede inicializaci seznamu L
*/
void InitList (tList *L) {

    L->First = NULL;
}

/**
* Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
* po inicializaci.
*/
void DisposeList (tList *L) {
    //pomocná proměnná
    tElemPtr elem;
    //kontrola jestli je seznam prázdný
    if(L->First != NULL)
    {
        //do elem si uložím hodnotu ke smazání a do aktivního prvku si uložím další prvek,
        //postupně jak procházím cyklem, mažu jednotlivé položky
        //nakonec zkončím, když je ativní prvek roven NULL a zbyvá už jen přenastavit First na NULL
        while(L->First->ptr != NULL)
        {
            elem = L->First->ptr;
            free(L->First->tPtr);
            free(L->First);
            L->First = elem;
        }
        L->First = NULL;
    }
}

/**
* Vloží prvek na začátek seznamu L.
* V případě, že není dostatek paměti pro nový prvek při operaci malloc,
* volá funkci Error().
*/
void InsertFirst (tList *L, tTNodePtr *trP) {
    tElemPtr elem;
    tTNodePtr elem2;
    if(L->First == NULL)
    {
        elem = malloc(sizeof(tElemPtr));
        if(elem == NULL)
        {
            Error();
        }
        else
        {
            elem2 = malloc(sizeof(tTNodePtr));
            if(elem2 == NULL)
            {
                Error();
            }
            else
            {
                elem2 = *trP;
                L->First = elem;
                L->First->tPtr = elem2;
                L->First->right = false;
                L->First->left = false;
            }
        }
    }
    else
    {
        elem = malloc(sizeof(tList));
        if(elem == NULL)
        {
            Error();
        }
        else
        {
            elem2 = malloc(sizeof(tTNodePtr));
            if(elem2 == NULL)
            {
                Error();
            }
            else
            {
                elem2 = *trP;
                elem->ptr = L->First;
                L->First = elem;
                L->First->tPtr = elem2;
                L->First->right = false;
                L->First->left = false;
            }
        }
    }
}

/**
* Zruší první prvek seznamu L a uvolní jím používanou paměť.
* Pokud byl seznam L prázdný, nic se neděje.
*/
void DeleteFirst (tList *L) {
    if(L->First == NULL)
    {
        tElemPtr elem;
        elem = L->First->ptr;
        free(L->First->tPtr);
        free(L->First);
        L->First = elem;
    }
}

int plusInt(int prv, int drh)
{
    return prv+drh;
}

int treePass(tTNodePtr trP)
{
    int level = 1;
    tList *list = NULL;
    tTNodePtr elem = malloc(sizeof(tTNodePtr));
    if(elem == NULL)
    {
        Error();
        return 99;
    }
    InitList(&*list);
    InsertFirst(&*list, &trP);
    while(level != 0)
    {
        if(list->First->left == false)
        {
            list->First->left = true;
            if(trP->LPtr == NULL)
            {

            }
            else
            {
                trP = trP->LPtr;
                level++;
                InsertFirst(&*list, &trP);
            }
        }
        else
        {
            if(list->First->right == false)
            {
                list->First->right = true;
                if(trP->LPtr == NULL)
                {

                }
                else
                {
                    trP = trP->RPtr;
                    level++;
                    InsertFirst(&*list, &trP);
                }
            }
            else
            {
                if(level > 1)
                {
                    trP = list->First->tPtr;
                }
                DeleteFirst(&*list);
                level--;
            }
        }
    }
    DisposeList(&*list);
    return 0;
 } 

//HERE ARE LIONS
 int interpretStart(); 
 int interpretEnd();
 char *mallocString(char *s);
 void addInternalFunctions(varTable table);
 void fGInsert(tTNodePtr root, char *s);
 void sGInsert(tTNodePtr root, char *s);
 int makeGlobalTable(tTNodePtr root);  
 tTNodePtr findEntryPoint(tTNodePtr root, char *c, char *f);
 char *getID(tTNodePtr root);
 void* evaluate(tTNodePtr root, int etype);
 int intEvaluate(tTNodePtr root);
 double doubleEvaluate(tTNodePtr root);
 char *stringEvaluate(tTNodePtr root);
 int isInternal(char *s);
 void substrHelp(tTNodePtr root, int *a, int *b);
 int executeInternal(char *c, char *f, tTNodePtr parameters);
 tableElemPtr functionCall(tTNodePtr root);
 varTable makeFunctionTable(char *c, char*f, tTNodePtr arguments, tTNodePtr parameters);
 void makeLocalVar(varTable localTable, tTNodePtr root);
 int executeFunction(char *c, char *f, tTNodePtr parameters);
 int doStatement(tTNodePtr root);
 int doBlock (tTNodePtr root);
 int doCycle (tTNodePtr root);
 int doCondition (tTNodePtr root);
 int doCall (tTNodePtr root);
 int doReturn (tTNodePtr root);
 int doAssignment (tTNodePtr root);
 double compare(tTNodePtr root); 



 varTable globalTable;
 varStack tableStack;
 extern tTNodePtr derivationTree;


 int interpretStart() {
     globalTable = VTinit(NULL, NULL, 0, 101);
     tableStack = VSinit();
     addInternalFunctions(globalTable);
     makeGlobalTable(derivationTree);
     int ret = executeFunction("Main.run", NULL, NULL);
     return ret;
 }


 int interpretEnd() {
     VTdispose(&globalTable);
     VSdispose(tableStack);
     //...
     return 0;
 }

 
 char *mallocString(char *s) {
     char* temp = malloc(sizeof(char)*(strlen(s)+1));
     strcpy(temp, s);
     return temp;
 }


 void addInternalFunctions(varTable table) {
     VTinsert(table, mallocString("ifj16.readInt"), 1, NULL);
     VTinsert(table, mallocString("ifj16.readDouble"), 2, NULL);
     VTinsert(table, mallocString("ifj16.readString"), 3, NULL);
     VTinsert(table, mallocString("ifj16.print"), 0, NULL);
     VTinsert(table, mallocString("ifj16.length"), 1, NULL);
     VTinsert(table, mallocString("ifj16.substr"), 3, NULL);
     VTinsert(table, mallocString("ifj16.compare"), 1, NULL);
     VTinsert(table, mallocString("ifj16.find"), 1, NULL);
     VTinsert(table, mallocString("ifj16.sort"), 3, NULL);
     VTinsert(table, mallocString("ifj16.IntToReal"), 2, NULL);
 }


 void fGInsert(tTNodePtr root, char *s) {

     char *temp = getID((root->LPtr)->RPtr);
     char *joined = malloc(sizeof(char)*(strlen(s)+strlen(temp)+2));
     strcpy(joined, s);
     joined = strcat(joined, ".");
     joined = strcat(joined, temp);
     int ftype = 0; 
     switch (((root->LPtr)->LPtr)->key) { 
         case INT_DATA: ftype = 1; break;
         case DOUBLE_DATA: ftype = 2; break;
         case STRING_DATA: ftype = 3; break;
     }
     VTinsert(globalTable, joined, ftype, NULL);
 }

  void sGInsert(tTNodePtr root, char *s) {

     char *temp = getID((root->LPtr)->RPtr);
     char *joined = malloc(sizeof(char)*(strlen(s)+strlen(temp)+2));
     strcpy(joined, s);
     joined = strcat(joined, ".");
     joined = strcat(joined, temp);
     int vtype = 0; 
     switch (((root->LPtr)->LPtr)->key) { 
         case INT_DATA: vtype = 4; break;
         case DOUBLE_DATA: vtype = 5; break;
         case STRING_DATA: vtype = 6; break;
     }
     void *ptr = evaluate(root->RPtr, vtype);
     VTinsert(globalTable, joined, vtype, ptr);
 }


 int makeGlobalTable(tTNodePtr root) {
 tTNodePtr temp = root;
 tTNodePtr other_temp = NULL;
 while (temp != NULL) {
     char *s = getID((temp->LPtr)->LPtr);
     other_temp = temp->LPtr;
     other_temp = other_temp->RPtr;
     while (other_temp != NULL) {
         if ((other_temp->LPtr)->key == FUNCTION) { 
             fGInsert(other_temp->LPtr, s);
         } else {
             sGInsert(other_temp->LPtr, s); 
         }
         other_temp = other_temp->RPtr;
     }
     temp = temp->RPtr;
 }
 return 0;
 }


 tTNodePtr findEntryPoint(tTNodePtr root, char *c, char *f) { 
 char *s =NULL;
 if (f == NULL) {
     int i = 0;
     f = strchr(c, 46) + 1;
     s = malloc(sizeof(char)*(strlen(c)-(strlen(f))));
     while (c[i] != '.') {
         s[i] = c[i];
         i++;
     }
     s[i] = '\0';     
 } else {
     s = c;
 }
   
 tTNodePtr temp = root;
 tTNodePtr other_temp = NULL;
 while (temp != NULL) {  
     other_temp = temp->LPtr;
     char *ID = getID(other_temp->LPtr);
     if (!strcmp(ID, s)) {
         
         other_temp = other_temp->RPtr; 
         while (other_temp != NULL) {
             ID = getID(((other_temp->LPtr)->LPtr)->RPtr);
             if (!strcmp(ID,f)) {
                 return other_temp->LPtr;
             }
             other_temp = other_temp->RPtr; 
         }
         break;
     }
     temp = temp->RPtr;
  }
  return NULL;
  }


 char *getID(tTNodePtr root) {
     return root->literal; 
 }


 void* evaluate(tTNodePtr root, int etype) {
     if (root == NULL) {
         return NULL;
     }
     int *temp = NULL;
     double *other_temp = NULL;
     char **strval; 
     switch (etype) {
         case 1:
         case 4:
             temp = malloc(sizeof(int));
             *temp = intEvaluate(root);
             return (void*) temp;  
         case 2:
         case 5: 
             other_temp = malloc(sizeof(double));
             *other_temp = doubleEvaluate(root);
             return (void*) other_temp;  
         case 3:
         case 6:
 
             return (void*) stringEvaluate(root); 
     }

 }


 int intEvaluate(tTNodePtr root) {
     int temp = 0;
     int other_temp = 0;
     if (root->key == EXPRESSION) {
         if (root->LPtr != NULL) {
             temp = intEvaluate(root->LPtr);
         }

         if (root->RPtr != NULL) {
             other_temp = intEvaluate(root->RPtr);
         }

         if (root->literal == NULL) {
             return temp;
         } else {
             if (!strcmp(root->literal,"+")) {
                 return temp + other_temp;
             } else {
                 return temp - other_temp;
             }
         }
     }

     if (root->key == TERM) {
         if (root->LPtr != NULL) {
             temp = intEvaluate(root->LPtr);
         }

         if (root->RPtr != NULL) {
             other_temp = intEvaluate(root->RPtr);
         }

         if (root->literal == NULL) {
             return temp;
         } else {
             if (!strcmp(root->literal,"*")) {
                 return temp * other_temp;
             } else {
                 return temp / other_temp;
             }
         }
     }

     if (root->key == INT) {
         return atoi(root->literal);
     }

     if (root->key == ID) {
         varTable localTable = VStop(tableStack);
         tableElemPtr temp = VTsearch(localTable, root->literal);
         if (temp != NULL) {
             int value = *((int*) temp->val);
             return value;
         }
     }


     if (root->key == CALL) {
         tableElemPtr help = functionCall(root);
         if (help != NULL) {   
             int value = *((int*) help->val);
             free(help->val);   
             return value;
         } else {
             return 0; 
         }
     }
 } 

  double doubleEvaluate(tTNodePtr root) {
     double temp = 0;
     double other_temp = 0;
     if (root->key == EXPRESSION) {
         if (root->LPtr != NULL) {
             temp = doubleEvaluate(root->LPtr);
         }

         if (root->RPtr != NULL) {
             other_temp = doubleEvaluate(root->RPtr);
         }

         if (root->literal == NULL) {
             return temp;
         } else {
             if (!strcmp(root->literal,"+")) {
                 return temp + other_temp;
             } else {
                 return temp - other_temp;
             }
         }
     }

     if (root->key == TERM) {
         if (root->LPtr != NULL) {
             temp = doubleEvaluate(root->LPtr);
         }

         if (root->RPtr != NULL) {
             other_temp = doubleEvaluate(root->RPtr);
         }

         if (root->literal == NULL) {
             return temp;
         } else {
             if (!strcmp(root->literal,"*")) {
                 return temp * other_temp;
             } else {
                 return temp / other_temp;
             }
         }
     }

     if (root->key == DOUBLE) {
         return strtod(root->literal, NULL);
     }

     if (root->key == ID) {
         varTable localTable = VStop(tableStack);
         tableElemPtr temp = VTsearch(localTable, root->literal);
         if (temp != NULL) {
             double value = *((double*) temp->val);
             return value;
         }
     }

     if (root->key == CALL) {
         tableElemPtr temp = functionCall(root);
         if (temp != NULL) {
             double value = *((double*) temp->val);
             free(temp->val);   
             return value;
         }
     }
 } 

 
 char *stringEvaluate(tTNodePtr root) {
     char *temp = NULL;
     char* other_temp = NULL;
     if (root->key == EXPRESSION) {
         if (root->LPtr != NULL) {
             temp = stringEvaluate(root->LPtr);
         }

         if (root->RPtr != NULL) {
             other_temp = stringEvaluate(root->RPtr);
         }

         if (root->literal == NULL) {
             return temp;
         } else {
             if (!strcmp(root->literal,"+")) {
                 char *sum = malloc(sizeof(char)*(strlen(temp)+strlen(other_temp)+1));
                 strcpy(sum, temp);
                 sum = strcat(sum, other_temp);
                 return sum;
             } else {
                 return NULL;
             }
         }
     }
     if (root->key == STRING) {
         temp = malloc(sizeof(char)*(strlen(root->literal)+1));
         strcpy(temp, root->literal); 
         return temp;
     }

     if (root->key == ID) {
         varTable localTable = VStop(tableStack);
         tableElemPtr temp = VTsearch(localTable, root->literal);
         if (temp != NULL) {
             char *value = malloc(sizeof(char)*(strlen((char*) temp->val)+1));
             strcpy(value, (char*) temp->val);
             return value;
         }
     } 

     if (root->key == CALL) {
         tableElemPtr temp = functionCall(root);
         if (temp != NULL) {
             char *value = (char*) temp->val;
             temp->val = NULL;   
             return value;
         }
     }
 }


 int isInternal(char *s) {
     if (strspn(s, "ifj16.") == 6) {
         return 1;
     } else {
         return 0;
     }
 }


 void substrHelp(tTNodePtr root, int *a, int *b) {
     *a = atoi((root->LPtr)->literal);
     root = root->RPtr;
     *b = atoi((root->LPtr)->literal);
 }


 void IFJ16_print(tTNodePtr parameters) {
     printf("%s", stringEvaluate(parameters->LPtr));
 }


 int executeInternal(char *c, char *f, tTNodePtr parameters) {

     tableElemPtr temp = VTsearch(globalTable, c); 

     if (!strcmp(c, "ifj16.IntToReal")) {
         temp->val = (void*) malloc(sizeof(double));
         *((double*) temp->val) = (double) intEvaluate(parameters->LPtr);
         return 0;
     } 
     if (!strcmp(c, "ifj16.readInt")) {
         temp->val = (void*) malloc(sizeof(int));
         ret = IFJ16_readInt((int*) temp->val);
         return ret;
     }
     if (!strcmp(c, "ifj16.readDouble")) {
         temp->val = (void*) malloc(sizeof(double));
         ret = IFJ16_readDouble((double*) temp->val);
         return ret;
     }
     if (!strcmp(c, "ifj16.readString")) {
         ret = IFJ16_readString((char**) &(temp->val) );
         return ret;
     }
     if (!strcmp(c, "ifj16.print")) {
         IFJ16_print(parameters);
         return 0;
     }
     if (!strcmp(c, "ifj16.length")) {
         temp->val = (void*) malloc(sizeof(int));
         *((int*)(temp->val)) = IFJ16_length(stringEvaluate(parameters->LPtr));
         return 0;
     }
     if (!strcmp(c, "ifj16.substr")) {
         int *i = malloc(sizeof(int));
         int *n = malloc(sizeof(int));
         substrHelp(parameters->RPtr, i, n);
         IFJ16_substr(stringEvaluate(parameters->LPtr), *i, *n, (char*) temp->val);
         return 0;
     }
     if (!strcmp(c, "ifj16.compare")) {
         char *first = stringEvaluate(parameters->LPtr);
         parameters = parameters->RPtr;
         char *second = stringEvaluate(parameters->LPtr);
         temp->val = (void*) malloc(sizeof(int));
         *((int*)(temp->val)) = IFJ16_compare(first, second); 
         return 0;
     }
     if (!strcmp(c, "ifj16.find")) {
         char *first = stringEvaluate(parameters->LPtr);
         parameters = parameters->RPtr;
         char *second = stringEvaluate(parameters->LPtr); 
         temp->val = (void*) malloc(sizeof(int));
         *((int*)(temp->val)) = IFJ16_find(first, second);
         return 0;
     }
     if (!strcmp(c, "ifj16.sort")) {
         char *first = stringEvaluate(parameters->LPtr);
         temp->val = (void*) malloc(sizeof(char)*(strlen(first)+1));
         ret = IFJ16_sort(first, (char*) temp->val);
         return 0;
     }
     
 }
 

 tableElemPtr functionCall(tTNodePtr root) {
     if (isInternal(getID(root->LPtr))) {
         int ret = executeInternal(getID(root->LPtr), NULL, root->RPtr);
     } else { 
         int ret = executeFunction(getID(root->LPtr), NULL, root->RPtr);
     }
     if (ret == 0) {
         return VTsearch(globalTable, getID(root->LPtr));
     }
 }


 varTable makeFunctionTable(char *c, char*f, tTNodePtr arguments, tTNodePtr parameters) {
     tableElemPtr fElem = VTsearch(globalTable, c);
     varTable temp = VTinit(c, f, fElem->type, 101);
     if (parameters == NULL) {
         return temp;
     }
     
     while (arguments != NULL && parameters != NULL) {
         char *id = getID((arguments->LPtr)->RPtr);
         int vtype = 0;
         switch (((arguments->LPtr)->LPtr)->key) { 
             case INT_DATA: vtype = 4; break;
             case DOUBLE_DATA: vtype = 5; break;
             case STRING_DATA: vtype = 6; break;
         }
         void *value = evaluate(parameters->LPtr, vtype);
         VTinsert(temp, id, vtype, value);
         arguments = arguments->RPtr;
         parameters = parameters->RPtr; 
     }    
     return temp;
 }


 void makeLocalVar(varTable localTable, tTNodePtr root) {
     tTNodePtr temp = root->LPtr;
     char *c = getID(temp->RPtr);
     int vtype = 0;
     switch (((root->LPtr)->LPtr)->key) { 
         case INT_DATA: vtype = 4; break;
         case DOUBLE_DATA: vtype = 5; break;
         case STRING_DATA: vtype = 6; break;
     }
     void *value = evaluate(root->RPtr, vtype);  
     VTinsert(localTable, c, vtype, value); 
 }


 int executeFunction(char *c, char *f, tTNodePtr parameters) {
     tTNodePtr start = findEntryPoint(derivationTree, c, f);  
     if (start == NULL) {
         return 10;
     }
     start = start->RPtr;
     varTable localTable = makeFunctionTable(c, f, start->LPtr, parameters);
     VSpush(tableStack, localTable); 
     start = start->RPtr; 
     if ( start->LPtr == NULL) {
         return 0;
     }

     while (start->LPtr != NULL) {
         if ((start->LPtr)->key == LOCAL_VAR) {
             makeLocalVar(localTable, start->LPtr);
         } else {
             int ret = doStatement((start->LPtr)->LPtr);
             if (ret) {
                 break;
             }
         }
         if (start->RPtr != NULL) {  
             start = start->RPtr;
         } else {
             break;
         }  
     }
     VSpop(tableStack); 
     return 0;  
 }


 int doStatement(tTNodePtr root) {

     switch (root->key) {
         case BLOCK_LIST:
             doBlock(root);
             break;
         case CONDITION:
             doCondition(root);
             break;
         case CYCLE:
             doCycle(root);
             break;
         case CALL:
             doCall(root); 
             break;
         case RETURN:
             doReturn(root);
             return 1; 
             break;
         case ASSIGNMENT:
             doAssignment(root);
             break;     
     }
     return 0;
 }


 int doBlock (tTNodePtr root) {
     while (root != NULL) {
         if (root->LPtr == NULL) {
             break;
         }
         if ((root->LPtr)->LPtr == NULL) {
             break;
         }
         int ret = doStatement((root->LPtr)->LPtr);
         if (ret) {
             return 1;
         }
         root = root->RPtr;
     }
     return 0;
 }

 int doCall (tTNodePtr root) {
     tableElemPtr temp = functionCall(root);
     free(temp->val);
     return 0;
 }

 
 int doCondition(tTNodePtr root) {
     tTNodePtr temp = root->RPtr;
     if (compare(root->LPtr)) {
         int ret = doBlock(temp->LPtr);
         if (ret) {
             return 1;
         } 
     } else {
         int ret = doBlock(temp->RPtr);
         if (ret) {
             return 1;
         }
     } 
     return 0;
 }


 int doCycle(tTNodePtr root) {
     while (compare(root->LPtr)) {  
         int ret = doBlock(root->RPtr);
         if (ret) {
             return 1;
         }
     }
     return 0;
 }

 
 int doReturn (tTNodePtr root) {
     varTable localTable = VStop(tableStack);
     tableElemPtr help = VTsearch(globalTable, localTable->c);
     help->val = evaluate(root->LPtr, localTable->type); 
     return 1; 
 }

 
 int doAssignment(tTNodePtr root) {
     varTable localTable = VStop(tableStack);
     tableElemPtr help = VTsearch(localTable, getID(root->LPtr));
     if (help == NULL) {
         help = VTsearch(globalTable, getID(root->LPtr));
     }
     help->val = evaluate(root->RPtr, help->type);
     return 0;
 }


 double compare(tTNodePtr root) {
     double *temp = NULL;
     double *other_temp = NULL;
     if (root->RPtr == NULL) {
         temp = (double*) evaluate(root->LPtr, 2);
         return *temp;
     } else {
         temp = (double*) evaluate(root->LPtr, 2);
         other_temp = (double*) evaluate(root->RPtr, 2);
     }

     if (!strcmp(root->literal,"==")) {
         return *temp == *other_temp;
     }
     if (!strcmp(root->literal,"!=")) {
         return *temp != *other_temp;
     }
     if (!strcmp(root->literal,">=")) {
         return *temp >= *other_temp;
     }
     if (!strcmp(root->literal,"<=")) {
         return *temp <= *other_temp;
     }
     if (!strcmp(root->literal,">")) {
         return *temp > *other_temp;
     }
     if (!strcmp(root->literal,"<")) {
         return *temp < *other_temp;
     }
 }

