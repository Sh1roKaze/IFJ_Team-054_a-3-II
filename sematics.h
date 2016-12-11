/*
Projekt IFJ16
 
xosker02 - Jan Oskera
*/

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

//build structure for call function int to real, replace int in tree to double
int subtree_int_to_real(tTNodePtr* ptr);
//Main function of sematic, load to HT, call and control result of other functions
int sematics(tTNodePtr ptr, IAL_HashTable *Table);
//first going throught tree, load static var and static functional to HT
int load_static (tTNodePtr ptr, IAL_HashTable *HTable);
//load inner functions to HT
int load_inner (IAL_HashTable *HTable);

#endif
