/*
Projekt IFJ16
xverny00 - Jan Verny
xvalac08 - Dominik Valachovic 
xosker02 - Jan Oskera
xvlach16 - Antonin Vlach
xzikmu08 - David Zikmund
*/

// stack.h
// 20. 11. 2016
// (c) Jan Oškera
// V rámci projektu do předmětů IFJ a IAL

#ifndef stack_
#define stack_

#include "parser.h"
#include <stdlib.h>

//type for item in stack struct
typedef tTNodePtr tItemStack;

//stack struct
typedef struct tStack{
      tItemStack item;
      struct tStack* next;     
}*tStackPtr;

//extern variable detect malloc error
extern int DetectStackError;

//init stack
void SInit (tStackPtr *S);
//push to stack
void SPush (tStackPtr *S, tItemStack ptr);
//pop from stack, return pointer tItemStack
tItemStack STopPop (tStackPtr *S);
//check if empty
int SEmpty (tStackPtr *S);
//destroy stack
void DStack (tStackPtr *S);
//read top
tItemStack STop (tStackPtr *S);

#endif
