// sematic.h
// 20. 11. 2016
// (c) Jan Oškera
// V rámci projektu do předmětů IFJ a IAL

#ifndef sematics_
#define sematics_

#include "parser.h"
#include "stack.h"
#include "ial.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int subtree_int_to_real(tTNodePtr* ptr);
int sematics(tTNodePtr ptr, IAL_HashTable **Table);
int load_static (tTNodePtr ptr, IAL_HashTable *HTable);
int load_inner (IAL_HashTable *HTable);

#endif
