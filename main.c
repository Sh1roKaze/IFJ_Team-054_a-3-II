//#include "ial.h"
#include "sematics.h"
#include "lex_anal.h"
#include "interpret.h"

int main(){
      int error = 0;
      tTNodePtr ptr = NULL;
      IAL_HashTable Table;
      IAL_HashTable *HTable = &Table;
      IAL_htItem *item = NULL;
      int ret;

      //error = sematics(ptr, &HTable);
      IAL_htInit(HTable);

      beginLexicalAnalysis();

      derivationTree = malloc(sizeof(struct tTNode));
      ret = file(derivationTree);
      if (ret == 0) {
          ret = sematics(derivationTree, &HTable);
          if (ret == 0) {
              ret = interpretStart();
              interpretEnd();
          } 
      }
 
      freeTree(&derivationTree);
      return ret;
}
