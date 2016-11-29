#ifndef INTERPRET
#define INTERPRET

#include <stdbool.h>
#include "parser.h"
#include "mem_management.h"
#include "ifj16.h"
#include "ial.h"

/* ořezaný jednosměrný seznam k uložení cesty seznamu */
typedef struct tElem {
    struct tElem *ptr;
    bool right;
    bool left;
    struct tTNode *tPtr;
} *tElemPtr;

typedef struct {
    tElemPtr First;
} tList;

void InitList (tList *);
void DisposeList (tList *);
void InsertFirst (tList *, tTNodePtr *);
void DeleteFirst (tList *);

//HERE ARE LIONS
int interpret();

#endif
