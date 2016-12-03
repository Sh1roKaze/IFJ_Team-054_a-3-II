#include "ial.h"
#include "sematics.h"
#include "lex_anal.h"
#include "interpret.h"

int main(){
      IAL_HashTable HTable;
      IAL_htItem *item = NULL;
      int ret;

      IAL_htInit(&HTable);

      beginLexicalAnalysis();

      derivationTree = malloc(sizeof(struct tTNode));
      ret = file(derivationTree);
      if (ret == 0) {
          ret = sematics(derivationTree, &HTable);
          //if (ret == 0) {
          //    ret = interpretStart();
          //    interpretEnd();
          //} 
      }
 
      IAL_htDispose(&HTable);
      freeTree(&derivationTree);
      return ret;
}
