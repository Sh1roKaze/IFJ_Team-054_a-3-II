#include "parser.h"
#include "stack.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int subtree_int_to_real(tTNodePtr* ptr);
int sematics(tTNodePtr ptr);
int load_static (tTNodePtr ptr, parsHT_Table *HTable);

