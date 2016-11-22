// stack.c
// 20. 11. 2016
// (c) Jan Oškera
// V rámci projektu do předmětů IFJ a IAL

#include "stack.h"

//extern variable detect malloc error
int DetectStackError;

//init stack
void SInit (tStackPtr *S)  
{
      DetectStackError = 0;
	*S = NULL;  
}	

//push to stack
void SPush (tStackPtr *S, tItemStack ptr)
{ 
      tStackPtr new = malloc(sizeof(struct tStack));
      if (new != NULL){
            new->item = ptr;
            new->next = *S;
            *S = new;      
	}
      else
            DetectStackError = 1;      
}

//pop from stack, return pointer tItemStack
tItemStack STopPop (tStackPtr *S)
{
      tStackPtr del;
      tItemStack back;
	if (*S != NULL)
		del = *S;
            *S = (*S)->next;
            back = del->item;
            free(del);
            return back;	
	}	
	else 
		return NULL;	
}

//read top
tItemStack STop (tStackPtr *S){
      if (SEmpty (S) != 1)
            return (*S)->item;
      else
            return NULL;
}

//check if empty
int SEmpty (tStackPtr *S)
{
  return (*S == NULL)? 1 : 0;
}	

//destroy stack
void DStack (tStackPtr *S){
      tStackPtr del;
      while (!SEmpty (S)){
            del = *S;
            *S = (*S)->next;
            free(del);
      }
            
}
