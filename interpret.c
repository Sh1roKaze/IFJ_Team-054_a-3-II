#include "interpret.h"

//HERE ARE LIONS
 int interpret(); 
 int interpretEnd();
 char *mallocString(char *s);
 void addInternalFunctions(varTable table);
 void fGInsert(tTNodePtr root, char *s);
 void sGInsert(tTNodePtr root, char *s);
 int makeGlobalTable(tTNodePtr root);  
 tTNodePtr findEntryPoint(tTNodePtr root, char *c);
 char *getID(tTNodePtr root);
 void* evaluate(tTNodePtr root, int etype);
 int intEvaluate(tTNodePtr root);
 double doubleEvaluate(tTNodePtr root);
 char *stringEvaluate(tTNodePtr root);
 int isInternal(char *s);
 void substrHelp(tTNodePtr root, int *a, int *b);
 int executeInternal(char *c, tTNodePtr parameters);
 tableElemPtr functionCall(tTNodePtr root);
 varTable makeFunctionTable(char *c, tTNodePtr parameters);
 void makeLocalVar(varTable localTable, tTNodePtr root);
 int executeFunction(char *c);
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


 /*Start interpretu */
 int interpret() {
     globalTable = VTinit(NULL, 0, 101); //inicializuje tabulku funkcí a globálních proměnných (globalTable)
     tableStack = VSinit();              //inicializuje zásobník pro tabuky lokálních proměnných
     addInternalFunctions(globalTable);  //zavede do globalTable jména vnitřních funkcí a jejich návratové hodnoty
     makeGlobalTable(derivationTree);    //zavede do globalTable všechny globální proměnné + jména všech funkcí a jejich návratové hodnoty
     VSpush(tableStack, makeFunctionTable("Main.run", NULL)); //vytvori prazdnou tabulku lokalnich promennych pro Main.run 
     int ret = executeFunction("Main.run"); //začne vykonávat funkci Main.run
     interpretEnd();                     //deinicializuje všechny tabulky proměnných 
     return ret;
 }

/*
konec interpretu
*/
 int interpretEnd() {
     VTdispose(&globalTable);
     VSdispose(tableStack);
     //...
     return 0;
 }

/*
allokace stringu
*/ 
 char *mallocString(char *s) {
     char* temp = malloc(sizeof(char)*(strlen(s)+1));
     strcpy(temp, s);
     return temp;
 }


/*
vlozi interni funkce do globalni tabulky
*/
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

 /* vloží do globalTable funkci
    root je kořen stromu deklarace funkce
    s je ukazatel na string se jménem třídy
    Položka funkce v tabulce se skládá z plně kvantifikovaného jména funkce,
    typu její návratové hodnoty 
    a ukazatele do paměti kde bude uložena návratová hodnota funkce po jejím zavolání
 */
 void fGInsert(tTNodePtr root, char *s) {

     // veme id a prida k nemu "." a jmeno tridy 
     char *temp = getID((root->LPtr)->RPtr);
     char *joined = malloc(sizeof(char)*(strlen(s)+strlen(temp)+2));
     strcpy(joined, s);
     joined = strcat(joined, ".");
     joined = strcat(joined, temp);

     //vyhodnoti datovy typ funkce
     int ftype = 0; 
     switch (((root->LPtr)->LPtr)->key) { 
         case INT_DATA: ftype = 1; break;
         case DOUBLE_DATA: ftype = 2; break;
         case STRING_DATA: ftype = 3; break;
     }

     //vlozi do tabulky polozku funkce
     VTinsert(globalTable, joined, ftype, NULL);
 }

/* vloží do globalTable globální proměnnou
    root je kořen stromu deklarace proměnné
    s je ukazatel na string se jménem třídy
    Položka globální proměnné v tabulce se skládá z plně kvantifikovaného jména proměnné,
    jejího datového typu 
    a ukazatele do paměti kde bude uložena její hodnota
 */
  void sGInsert(tTNodePtr root, char *s) {

     // veme id a prida k nemu "." a jmeno tridy
     char *temp = getID((root->LPtr)->RPtr);
     char *joined = malloc(sizeof(char)*(strlen(s)+strlen(temp)+2));
     strcpy(joined, s);
     joined = strcat(joined, ".");
     joined = strcat(joined, temp);

     //vyhodnoti datovy typ funkce
     int vtype = 0; 
     switch (((root->LPtr)->LPtr)->key) { 
         case INT_DATA: vtype = 4; break;
         case DOUBLE_DATA: vtype = 5; break;
         case STRING_DATA: vtype = 6; break;
     }
    
     //vyhodnoti vyraz 
     void *ptr = evaluate(root->RPtr, vtype);    
     VTinsert(globalTable, joined, vtype, ptr);
 }

/*
projde strom a vytvoří položku v globalTable pro každou funkci a proměnnou
*/
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

/*
vyhledá ve stromu vstupní uzel funkce
*/
 tTNodePtr findEntryPoint(tTNodePtr root, char *c) {

//rozdeli plne kvantifikovany identifikator na jmeno funkce a promenne  
 char *s = NULL;
 char *f = NULL;
 int i = 0;
 f = strchr(c, 46) + 1;
 s = malloc(sizeof(char)*(strlen(c)-(strlen(f))));
 while (c[i] != '.') {
     s[i] = c[i];
     i++;
 }
 s[i] = '\0';     
   
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

/*
vyhodnotí výraz typu zadaného pomocí "etype"
*/
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

/*
rekurzivní vyhodnocení výrazu typu int
*/
 int intEvaluate(tTNodePtr root) {
     int temp = 0;
     int other_temp = 0;
  
     //vyhodnocuje vyraz (plus/minus)
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

     //vyhodnocuje term (krat/deleno)
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

     //literal
     if (root->key == INT) {
         return atoi(root->literal);
     }

     //najde a vrati hodnotu promenne
     if (root->key == ID) {
         varTable localTable = VStop(tableStack);
         tableElemPtr temp = VTsearch(localTable, root->literal);
         if (temp == NULL) {
             temp = VTsearch(globalTable, root->literal);
         }
         if (temp->val == NULL) {
                 exit(8); 
         } 
         if (temp != NULL) {
             int value = *((int*) temp->val);
             return value;
         }
     }

     //zpracuje funkci
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

/*
rekurzivní vyhodnocení výrazu typu double
*/
  double doubleEvaluate(tTNodePtr root) {
     double temp = 0;
     double other_temp = 0;
    
     //plus nebo minus
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

     //nasobeni nebo deleni
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

     //literal
     if (root->key == DOUBLE) {
         return strtod(root->literal, NULL);
     }

     //vraci hodnotu promenne
     if (root->key == ID) {
         varTable localTable = VStop(tableStack);
         tableElemPtr temp = VTsearch(localTable, root->literal);
         if (temp == NULL) {
             temp = VTsearch(globalTable, root->literal);
         }
         if (temp->val == NULL) {
                 exit(8); 
         }
         if (temp != NULL) {
             double value = *((double*) temp->val);
             return value;
         }
     }

     //vyhodnoti funkci
     if (root->key == CALL) {
         tableElemPtr temp = functionCall(root);
         if (temp != NULL) {
             double value = *((double*) temp->val);
             free(temp->val);   
             return value;
         }
     }
 } 

/*
rekurzivní vyhodnocení výrazu typu string
*/ 
 char *stringEvaluate(tTNodePtr root) {
     char *temp = NULL;
     char* other_temp = NULL;

     // jenom plus
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

     //string literal nebo int literal
     if (root->key == STRING || root->key == INT || root->key == DOUBLE) {
         temp = malloc(sizeof(char)*(strlen(root->literal)+1));
         strcpy(temp, root->literal); 
         return temp;
     }

     //vrati hodnotu promenne
     if (root->key == ID) {
         varTable localTable = VStop(tableStack);
         tableElemPtr temp = VTsearch(localTable, root->literal);
         if (temp == NULL) {
             temp = VTsearch(globalTable, root->literal);
         }

         if (temp != NULL) {
             if (temp->val == NULL) {
                 exit(8); 
             }

             if (temp->type == 6) {
             char *value = malloc(sizeof(char)*(strlen((char*) temp->val)+1));
             strcpy(value, (char*) temp->val);
             return value;
             }
             if (temp->type == 5) {
                 char *value = malloc(sizeof(char)*18);
                 snprintf(value, 17, "%g", *((double*) temp->val));
                 return value;  
             }
             if (temp->type == 4) {
                 char *value = malloc(sizeof(char)*18);
                 snprintf(value, 17, "%d", *((int*) temp->val));
                 return value; 
             }
         }
     } 

     //vyhodnoti funkci
     if (root->key == CALL) {
         tableElemPtr temp = functionCall(root);
         if (temp != NULL) {
             char *value = (char*) temp->val;
             temp->val = NULL;   
             return value;
         }
     }
 }

/*
relativně rychlé určení zda je volaná funkce vnitřní nebo ne
*/
 int isInternal(char *s) {
     if (strstr(s,"ifj16.") != NULL) {
         return 1;
     } else {
         return 0;
     }
 }

//pomocná funkce pro zpracování argumentání při volání ifj16.substr();
 void substrHelp(tTNodePtr root, int *a, int *b) {
     *a = atoi((root->LPtr)->literal);
     root = root->RPtr;
     *b = atoi((root->LPtr)->literal);
 }

/*
implementace funkce ifj16.print()
*/
 void IFJ16_print(tTNodePtr parameters) {
     //TODO
     printf("%s", stringEvaluate(parameters->LPtr));
 }

/*
univerzální funkce pro vykonání vnitřní funkce
*/
 int executeInternal(char *c, tTNodePtr parameters) {

      //TODO

     tableElemPtr temp = VTsearch(globalTable, c); 

     if (!strcmp(c, "ifj16.IntToReal")) {
         temp->val = (void*) malloc(sizeof(double));
         *((double*) temp->val) = (double) intEvaluate(parameters->LPtr);
         return 0;
     } 
     if (!strcmp(c, "ifj16.readInt")) {
         temp->val = (void*) malloc(sizeof(int));
         ret = IFJ16_readInt((int*) temp->val);
         if (ret == 7) {
             exit(ret);
         }
         return ret;
     }
     if (!strcmp(c, "ifj16.readDouble")) {
         temp->val = (void*) malloc(sizeof(double));
         ret = IFJ16_readDouble((double*) temp->val);
         if (ret == 7) {
             exit(ret);
         }
         return ret;
     }
     if (!strcmp(c, "ifj16.readString")) {
         ret = IFJ16_readString((char**) &(temp->val) );
         if (ret == 99) {
             exit(ret);
         }
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
 
/*
funkce pro volání funkce během běhu interpretu
*/
 tableElemPtr functionCall(tTNodePtr root) {
     int ret = 0;
     if (isInternal(getID(root->LPtr))) {
         ret = executeInternal(getID(root->LPtr), root->RPtr);
     } else {
         VSpush(tableStack, makeFunctionTable(getID(root->LPtr), root->RPtr)); 
         ret = executeFunction(getID(root->LPtr));
     }
     
     return VTsearch(globalTable, getID(root->LPtr));
 }

/*
založí tabulku lokálních proměnných a přiřadí argumentům funkce jejich parametry

*/
 varTable makeFunctionTable(char *c, tTNodePtr parameters) {

    //zalozi prazdnou tabulku lokalnich promennych, pokud fce nema parametry, skonci 
     tableElemPtr fElem = VTsearch(globalTable, c);
     varTable temp = VTinit(c, fElem->type, 101);
     if (parameters == NULL) {
         return temp;
     }
     
     if (parameters->LPtr == NULL) {
         return temp;

     }
 
     //najde uzel s argumenty
     tTNodePtr arguments = findEntryPoint(derivationTree, c);
     arguments = arguments->RPtr;
     arguments = arguments->LPtr;

     //priradi argumentum jejich vyhodnocene parametry
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
     //vraci hotovou tabulku lokalnich promennych     
     return temp;
 }

/*
vytvoří lokální proměnnou
*/
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

/*
provede volanou funkci
prakticky hlavní tělo interpretu 
*/
 int executeFunction(char *c) {
     int ret = 0;
     //pro zavolanou funkci najde vstupní bod
     tTNodePtr start = findEntryPoint(derivationTree, c);  
     if (start == NULL) {
         return 10;
     }  
     start = start->RPtr; 
     //vezme si svoji predvytvorenou tabulku lokalnich promennych
     //i s predanymi parametry
     varTable localTable = VStop(tableStack);
     start = start->RPtr; 
     if ( start->LPtr == NULL) {
         return ret;
     }

     //provádí jednotlivé uzly STATEMENT nebo zakládá nové lokální proměnné
     while (start->LPtr != NULL) {
         if ((start->LPtr)->key == LOCAL_VAR) {
             makeLocalVar(localTable, start->LPtr);
         } else {
             ret = doStatement((start->LPtr)->LPtr);
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
     return ret;  
 }


/*
Provede statement
*/
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

/*
provede blok prikazu
*/
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

/*
volání funkce a zahození její návratové hodnoty
*/
 int doCall (tTNodePtr root) {
     tableElemPtr temp = functionCall(root);
     if (temp != NULL) {
         free(temp->val);
     }
     return 0;
 }

/*
podminka
*/ 
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

/*
cyklus
*/
 int doCycle(tTNodePtr root) {
     while (compare(root->LPtr)) {  
         int ret = doBlock(root->RPtr);
         if (ret) {
             return 1;
         }
     }
     return 0;
 }

 
/*
return
*/
 int doReturn (tTNodePtr root) {
     varTable localTable = VStop(tableStack);
     tableElemPtr help = VTsearch(globalTable, localTable->c);
     help->val = evaluate(root->LPtr, localTable->type); 
     return 1; 
 }

/*
prirazeni do promenne
*/ 
 int doAssignment(tTNodePtr root) {
     varTable localTable = VStop(tableStack);
     tableElemPtr help = VTsearch(localTable, getID(root->LPtr));
     if (help == NULL) {
         help = VTsearch(globalTable, getID(root->LPtr));
     }
     help->val = evaluate(root->RPtr, help->type);
     return 0;
 }

/*
vyhodnoceni porovnani
*/
 double compare(tTNodePtr root) {
     double *temp = NULL;
     double *other_temp = NULL;
     temp = (double*) evaluate(root->LPtr, 2);
     other_temp = (double*) evaluate(root->RPtr, 2); 

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

