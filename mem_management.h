/*
Projekt IFJ16
xverny00 - Jan Verny
xvalac08 - Dominik Valachovic 
xosker02 - Jan Oskera
xvlach16 - Antonin Vlach
xzikmu08 - David Zikmund
*/

#ifndef MEM_MANAGEMENT
#define MEM_MANAGEMENT
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//VARTABLE
typedef struct tableElem {
    struct tableElem *next;
    char *id;
    int type;
    void *val;
} *tableElemPtr;

typedef struct tTable {
       char *c;
       char *f;
       int type;
       tableElemPtr *array;
       int size; 
} *varTable;


varTable VTinit(char *c, int type, int size);
tableElemPtr VTsearch(varTable table, char *s);
int VTinsert(varTable table, char *s, int type, void* value);
void VTdispose(varTable *table);

//VARSTACK
typedef struct stackElem {
    varTable item;
    struct stackElem *next; 
} *stackElemPtr;

typedef struct vStack {
    stackElemPtr top;
} *varStack;


varStack VSinit();
int VSempty(varStack stack);
varTable VStop(varStack stack);
int VSpush(varStack stack, varTable item);
int VSpop(varStack stack);
void VSdispose(varStack stack);

#endif






























































