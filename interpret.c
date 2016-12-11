#include "interpret.h"
#include "stack.h"
#include <math.h>

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
 int printEvaluate(tTNodePtr root, void **output);
 int term(tTNodePtr root, void **result);
 int handleNode(tTNodePtr root, void **result);
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
     if (ret == 1) {
         ret = 0;
     }
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
     char* temp = calloc((strlen(s)+1), sizeof(char));
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
     char *joined = calloc((strlen(s)+strlen(temp)+2), sizeof(char));
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
     char *joined = calloc((strlen(s)+strlen(temp)+2), sizeof(char));
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
     void *ptr = NULL;
     if (root->RPtr != NULL ) {
         ptr = evaluate(root->RPtr, vtype);
     }     
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
 s = calloc((strlen(c)-strlen(f)), sizeof(char));
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
     void** strval = NULL;
     char* retstr = NULL;
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
             strval = malloc(sizeof(void*));
             printEvaluate(root, strval);
             retstr = calloc((strlen((char*) *strval)+1), sizeof(char));
             strcpy(retstr, (char*) *strval);
             free(*strval);
             free(strval); 
             return (void*) retstr; 
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
                 if (other_temp == 0)
                     exit(9);
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
         if (temp == NULL || temp->val == NULL) {
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
                 if (fabs(other_temp) < 10e-8)
                     exit(9); 
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
         if (temp != NULL && temp->val == NULL) {
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
                 char *sum = calloc((strlen(temp)+strlen(other_temp)+1), sizeof(char));
                 strcpy(sum, temp);
                 sum = strcat(sum, other_temp);
                 return sum;
             } else {
                 if (!strcmp(root->literal,"-")) {
                     exit(4);
                 }
                 return NULL;
             }
         }
     }

     //string literal nebo int literal
     if (root->key == STRING || root->key == INT || root->key == DOUBLE) {
         temp = calloc((strlen(root->literal)+1), sizeof(char));
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
             char *value = calloc((strlen((char*) temp->val)+1), sizeof(char));
             strcpy(value, (char*) temp->val);
             return value;
             }
             if (temp->type == 5) {
                 char *value = calloc(18, sizeof(char));
                 snprintf(value, 17, "%g", *((double*) temp->val));
                 return value;  
             }
             if (temp->type == 4) {
                 char *value = calloc(18, sizeof(char));
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
vyhodnoti vyraz pro print
*/

int printEvaluate(tTNodePtr root, void **output) {
 int bString = 0;
 int retval = 0;
 int prev = 0; 
 char *first = NULL;
 char *second = NULL;
 char *temp = NULL; 
 void **fuck = malloc(sizeof(void*));
 double result = 0;

 tStackPtr *simpleS = malloc(sizeof(struct tStack));
 SInit(simpleS);

 if (root == NULL) {
     return 10;
 }

 while (root->key == EXPRESSION) {
     SPush(simpleS, root);
     root = root->LPtr;
 } 

 if (root->key == TERM) {
     retval = term(root, fuck);
 } else {
     retval = handleNode(root, fuck);
 }

 if (retval == 6) {
     first = (char*) *fuck;
     bString = 1;
 } else {
     result = *((double*) *fuck);
 }

 root = STopPop(simpleS);

 do {      
      if (root->RPtr != NULL) {
          if ((root->RPtr)->key == TERM) {
              retval = term(root->RPtr, fuck);
              if (retval == 6) {
                  exit(4);
              }
          } else {  
              retval = printEvaluate(root->RPtr, fuck); 
          }
      

      if (retval == 6 && bString == 0) {
          first = calloc(18, sizeof(char));
          snprintf(first, 17, "%g", result);
          bString = 1;
      }
  
      if (bString) {
          if (root->literal != NULL && (root->literal)[0] == '+') {
              if (retval == 5) {
              second = calloc(18, sizeof(char));
              snprintf(second, 17, "%g", *((double*) *fuck));
              } else {
                  second = (char*) *fuck;
              }
              temp = calloc((strlen(first)+strlen(second)+1), sizeof(char));
              strcpy(temp, first);
              strcat(temp, second);
              free(first);
              free(second);
              first = temp;
          }
          if (root->literal != NULL && (root->literal)[0] == '-') {
              exit(4);
          }
      } else {
          if (root->literal != NULL && (root->literal)[0] == '+') {
              result = result + *((double*) *fuck);
          } else {
              result = result - *((double*) *fuck);
          }
      }
      }

      root = STopPop(simpleS); 
     
 } while (!SEmpty(simpleS));
 
 DStack(simpleS);
 simpleS = NULL;

 if (bString) {
     *output = (void*) first;
     return 6;  
 } else {
     *((double*) *fuck) = result; 
     *output = *fuck;
     return 5; 
 }

}


int term(tTNodePtr root, void **result) {
 
 tStackPtr *simpleS = malloc(sizeof(struct tStack));
 SInit(simpleS); 
 double *dTemp = malloc(sizeof(double));
 void **screw = malloc(sizeof(void*));

 if (root == NULL) {
     return 10;
 }

 while (root != NULL) {
     SPush(simpleS,root);
     root = root->RPtr;
 }

 root = STopPop(simpleS);

 if ((root->LPtr)->key == EXPRESSION) {
     *dTemp = doubleEvaluate(root->LPtr);
 } else {
     int ret = handleNode(root->LPtr, screw);
     if (ret == 6) {
         exit(4);
     } else {
         *dTemp = *((double*) *screw);
     }
 }

 root = STopPop(simpleS);

 while (root != NULL) {

     double dHelp = 0; 

     if ((root->LPtr)->key == EXPRESSION) {
         dHelp = doubleEvaluate(root->LPtr);
     } else {
         int ret = handleNode(root->LPtr, screw);
         if (ret == 6) {
             exit(4);
         } else {
             dHelp = *((double*) *screw);
         }
     }

     if ((root->literal)[0] == '*') {
         *dTemp = *dTemp * dHelp;
     } else {
         *dTemp = *dTemp / dHelp;
     }
     
     root = STopPop(simpleS);
 }

 DStack(simpleS);
 simpleS = NULL;

 *((double*) *screw) = *dTemp; 
 *result = *screw;

 return 5;

}

int handleNode(tTNodePtr root, void **result) {

 if (root->key == INT) {
     *result = malloc(sizeof(double));
     *((double*) *result) = (double) atoi(root->literal);
     return 5;
 }

 if (root->key == DOUBLE) {
     *result = malloc(sizeof(double));
     *((double*) *result) = strtod(root->literal, NULL);
     return 5;
 }

 if (root->key == STRING) {
     char* temp = malloc(sizeof(char)*(strlen(root->literal)+1));
     strcpy(temp, root->literal);
     *result = (void*) temp;
     return 6;
 }

 if (root->key == ID) {
     varTable localTable = VStop(tableStack);
     tableElemPtr temp = VTsearch(localTable, root->literal);
     if (temp == NULL) {
         temp = VTsearch(globalTable, root->literal);
     }

     if (temp->val == NULL) {
         exit(8); 
     }

     if (temp->type == 6) {
         *result = (void*) calloc((strlen((char*) temp->val)+1), sizeof(char));
         strcpy((char*) *result, (char*) temp->val);
         return 6;    
     }
     if (temp->type == 5) {
         *result = malloc(sizeof(double));
         *((double*) *result) = *((double*) temp->val);
         return 5;           
     }
     if (temp->type == 4) {
         *result = (void*) malloc(sizeof(double));
         *((double*) *result) = (double) *((int*) temp->val);
         return 5; 
     }
          
 }

 if (root->key == CALL) { 
     tableElemPtr temp = functionCall(root);
     if (temp->val == NULL) {
         exit(8); 
     }

     if (temp->type == 3) {
         *result = (char*) temp->val;
         return 6;    
     }
     if (temp->type == 2) {
         *result = malloc(sizeof(double));  
         *((double*) *result) = *((double*) temp->val);
         return 5;            
     }
     if (temp->type == 1) {
         *result = malloc(sizeof(double));
         *((double*) *result) = (double) *((int*) temp->val);
         return 5; 
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
     *a = intEvaluate(root->LPtr);
     root = root->RPtr;
     *b = intEvaluate(root->LPtr);
 }

/*
implementace funkce ifj16.print()
*/
 void IFJ16_print(tTNodePtr parameters) {
     void **temp = malloc(sizeof(void*));
     int x = printEvaluate(parameters->LPtr, temp);
     if (x == 6) {
         printf("%s", (char*) *temp);    
     }
     if (x == 5) {
         printf("%g", *((double*) *temp));
     }
     free(*temp);
     free(temp);
 }

/*
univerzální funkce pro vykonání vnitřní funkce
*/
 int executeInternal(char *c, tTNodePtr parameters) {

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
         void **str = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, str);
         *((int*)(temp->val)) = IFJ16_length((char*) *str);
         free(*str);
         free(str);
         return 0;
     }
     if (!strcmp(c, "ifj16.substr")) {
         int *i = malloc(sizeof(int));
         int *n = malloc(sizeof(int));
         substrHelp(parameters->RPtr, i, n);
         void **str = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, str);
         temp->val = (void*) malloc(sizeof(char)*(strlen((char*) *str)+1));
         ret = IFJ16_substr((char*) *str, (unsigned) *i, (unsigned) *n, (char*) temp->val);
         free(*str);
         free(str);
         if (ret == 1) {
             exit(8);
         }
         if (ret == 10) {
             exit(10);
         }
         return 0;
     }
     if (!strcmp(c, "ifj16.compare")) {
         void **cmpFirst = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, cmpFirst);
         parameters = parameters->RPtr;
         void **cmpSecond = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, cmpSecond);
         temp->val = (void*) malloc(sizeof(int));

         *((int*)(temp->val)) = IFJ16_compare((char*) *cmpFirst, (char*) *cmpSecond);
         
         free(*cmpFirst);
         free(cmpFirst);
         free(*cmpSecond);
         free(cmpSecond); 
         return 0;
     }
     if (!strcmp(c, "ifj16.find")) {
         void **cmpFirst = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, cmpFirst);
         parameters = parameters->RPtr;
         void **cmpSecond = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, cmpSecond); 
         temp->val = (void*) malloc(sizeof(int));
         *((int*)(temp->val)) = IFJ16_find((char*) *cmpFirst, (char*) *cmpSecond);
         
         free(*cmpFirst);
         free(cmpFirst);
         free(*cmpSecond);
         free(cmpSecond);
         return 0;
     }
     if (!strcmp(c, "ifj16.sort")) {
         void **str = malloc(sizeof(void*));
         printEvaluate(parameters->LPtr, str);
         temp->val = (void*) malloc(sizeof(char)*(strlen((char*) *str)+1));
         ret = IFJ16_sort((char*) *str, (char*) temp->val);
         free(*str);
         free(str);
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
     tableElemPtr temp = VTsearch(globalTable, c);

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
         if (ret) {
             if (temp->type == 0) {
                  if (temp->val != NULL) 
                      exit(10);
             }
         } else {
             if (temp->type != 0) 
                 exit(8);
         }
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

     if (ret) {
         if (temp->type == 0) {
             if (temp->val != NULL) 
                  exit(10);
         }
     } else {
         if (temp->type != 0)
             exit(8);
     } 
     return ret;  
 }


/*
Provede statement
*/
 int doStatement(tTNodePtr root) {

     int ret = 0;
     switch (root->key) {
         case BLOCK_LIST:
             ret = doBlock(root);
             break;
         case CONDITION:
             ret = doCondition(root);
             break;
         case CYCLE:
              ret = doCycle(root);
             break;
         case CALL:
             ret = doCall(root); 
             break;
         case RETURN:
             ret = doReturn(root); 
             break;
         case ASSIGNMENT:
             ret = doAssignment(root);
             break;     
     }
     return ret;
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


