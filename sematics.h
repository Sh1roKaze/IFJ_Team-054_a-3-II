// sematic.h
// 20. 11. 2016
// (c) Jan Oškera
// V rámci projektu do předmětů IFJ a IAL

#include "parser.h"
#include "stack.h"
#include "ial.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int subtree_int_to_real(tTNodePtr* ptr);
int sematics(tTNodePtr ptr);
int load_static (tTNodePtr ptr, parsHT_Table *HTable);
int load_inner (IAL_HashTable *HTable);


