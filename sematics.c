/*
Projekt IFJ16
 
xosker02 - Jan Oskera
*/

// sematic.c
// 22. 11. 2016
// (c) Jan Oškera
// V rámci projektu do předmětů IFJ a IAL

#include "sematics.h"

//detect stack error 99
int DetectStackError;
//Actual class
static char *ActClass;
//error
static int error = 0;
//pointer on item from HT 
static IAL_htItem *item = NULL;


//Prototypes
int subtree_int_to_real(tTNodePtr* ptr);
int sematics(tTNodePtr ptr, IAL_HashTable *Table);
int function_control(tTNodePtr ptr, IAL_HashTable *HTable);
int st_list_control (tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes);
int statement_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes);
int block_list_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes);
int call_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *returns);
int expression_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char typ, int special);
int load_inner (IAL_HashTable *HTable);
int load_inner_function (IAL_HashTable *HTable, char *id, char *t);
int load_static (tTNodePtr ptr, IAL_HashTable *HTable);
int function_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable);
int static_var_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable);
static inline int try_find_global_load_item(tTNodePtr ptr, IAL_HashTable *HTable, char *name);
static inline void load_type_literal (node_t key, char *typ);
static inline char* add_class_before_name (char *ActClass, char *name);
static inline void load_type (node_t key, char *typ);
static inline char* add_type_before_types (char typ, char *types);
static inline char *add_char_behind_types (char *types, char c);
static inline int check_exist_function(IAL_HashTable *HTable, char *name);
static inline tTNodePtr push_right_go_left (tTNodePtr ptr, tStackPtr *S);
int string_control(tStackPtr *S, char *types, int special);
int double_control(tStackPtr *S, char **type);

/*build structure for call function int to real, replace int in tree to double*/
int subtree_int_to_real(tTNodePtr* ptr){
      tTNodePtr id_f = malloc (sizeof(struct tTNode));
      tTNodePtr par_list = malloc (sizeof(struct tTNode));
      tTNodePtr id_p = malloc (sizeof(struct tTNode));
      
      if (id_f != NULL && par_list != NULL && id_p != NULL){        
            id_f->LPtr = NULL;
            id_f->RPtr = NULL;
            id_f->key = ID;
            id_f->literal = malloc((strlen("IFJ16.IntToReal")+1)*sizeof(char));
            strcpy(id_f->literal,"ifj16.IntToReal");
            
            par_list->LPtr = id_p;
            par_list->RPtr = NULL;
            par_list->key = PAR_LIST;
            par_list->literal = NULL;

            id_p->LPtr = (*ptr)->LPtr;
            id_p->RPtr = (*ptr)->RPtr;
            id_p->key = (*ptr)->key;
            id_p->literal = (*ptr)->literal;
            
            (*ptr)->LPtr = id_f;
            (*ptr)->RPtr = par_list;
            (*ptr)->key = CALL;
            (*ptr)->literal = NULL;
                  
      }
      else{
            free(id_f);
            free(par_list);
            free(id_p);
            return(99);
      }

      return 0;
}

/* Main function of sematic, load to HT, call and control result of other functions */
int sematics(tTNodePtr ptr, IAL_HashTable *HTable){
      tStackPtr stack;      
      tStackPtr *S = &stack;      
      char typ;
      int rate = 0;

      error = load_inner (HTable);
      if (error != 0){
            return error;                  
      }

      error = load_static (ptr, HTable);
      if (error != 0){
            return error;                  
      }
      
      //going throught tree and evaluates errors
      SInit (S);
      SPush (S, ptr);
      do{
            ptr = STopPop (S);
            //CLASS_LIST
            if (ptr != NULL && ptr->key == CLASS_LIST){
                  ptr = push_right_go_left (ptr, S);
            }
            //CLASS
            if (ptr != NULL && ptr->key == CLASS){
                  ptr = push_right_go_left (ptr, S);
                  ActClass = ptr->literal;
                  rate = 0;
                  continue;          
            }
            //CLASS_ITEM
            if (ptr != NULL && ptr->key == CLASS_ITEM){
                  if (ptr->LPtr != NULL)
                        ptr = push_right_go_left (ptr, S);  
            }
            //STATIC_VAR
            if (ptr != NULL && ptr->key == STATIC_VAR){
                  rate++;
                  ptr = push_right_go_left (ptr, S);
                  //DECLARATION
                  ptr = ptr->LPtr;
                  //TYPE
                  load_type (ptr->key, &typ);
                  if ( SEmpty (S) != 1 && STop(S)->key == EXPRESSION ){
                        ptr = STopPop (S);
                        //EXPRESSION
                        error = expression_control(ptr, HTable, NULL, typ, rate); 
                        if (error != 0){
                              DStack (S);
                              return error;                        
                        }     
                  }
            }
            //FUNCTION
            if (ptr != NULL && ptr->key == FUNCTION){
                  error = function_control(ptr, HTable);
                  if (error != 0){
                        DStack (S);
                        return error;                        
                  }  
            }
      }while (!SEmpty (S));
      
      if (DetectStackError != 1)
            return 0;
      else
            return 99;
}

/* Check if exist function in HT */
static inline int check_exist_function(IAL_HashTable *HTable, char *name){
      char *fullname;
      
      fullname = add_class_before_name (ActClass, name);
      if (fullname == NULL){
            return (error == 99)? 99 : 2;
      } 

      item = IAL_htSearch(HTable, fullname);
      if (item != NULL){//Check local var name and function
            if (item->types[0] == 'F'){
                  free(fullname);
                  return 3;
            }
       }

       free(fullname);
       return 0;
}

/* Control function */
int function_control(tTNodePtr ptr, IAL_HashTable *HTable){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      char *name;
      char *vartypes;
      char *types;
      int i = 2;
      IAL_HashTable Table;
      IAL_HashTable *LHTable = &Table;
      IAL_htInit(LHTable);

      SInit (S);
      //FUNCTION
      ptr = push_right_go_left (ptr, S);
      //DECLARATION
      ptr = ptr->RPtr;
      //ID
      name = ptr->literal;
      name = add_class_before_name (ActClass, name);
      if (name == NULL){
            DStack (S);   
            IAL_htDispose(LHTable); 
            return (error == 99) ? 99 : 2;       
      }
      item = IAL_htSearch(HTable, name);
      free(name);      
      types = item->types;
      ptr = STopPop (S);
      //FUNCTION 2
      ptr = push_right_go_left (ptr, S);
      //ARG_LIST
      //control arguments
      SPush (S, ptr);
      while(SEmpty (S) != 1 && STop(S)->key == ARG_LIST){ //Load params to types
            ptr = STopPop (S);
            if (ptr->LPtr != NULL){
                  ptr = push_right_go_left (ptr, S);
                  //DECLARATION
                  ptr = ptr->RPtr;
                  //ID
                  name = ptr->literal;

                  error = check_exist_function(HTable, name);
                  if (error != 0){
                        DStack (S);
                        IAL_htDispose(LHTable);
                        return error;
                  }

                  if (IAL_htSearch(LHTable, name) == NULL){
                        vartypes = add_char_behind_types ("P", types[i]);
                        error = IAL_htInsert(LHTable, name, 0, vartypes);
                        if (error != 0){
                              DStack (S);
                              IAL_htDispose(LHTable);
                              return 99;
                        }
                        i++;
                        free(vartypes);
                  }
                  else{
                        DStack (S);
                        IAL_htDispose(LHTable);
                        return 3;      
                  }                           
            }
      }
      ptr = STopPop (S);
      
      //ST_LIST       
      if (ptr != NULL && ptr->key == ST_LIST){
            error = st_list_control (ptr, HTable, LHTable, types);
            if (error != 0){
                  DStack (S);
                  IAL_htDispose(LHTable);
                  return error;
            }
      }
      
      IAL_htDispose(LHTable);
      return 0;      
}

/* Control inner part of function (local var, etc..)*/ 
int st_list_control (tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      char *name;
      char typ;
      char *types;

      SInit (S);
      SPush (S, ptr);
      
      do{
            ptr = STopPop (S);
            //ST_LIST
            if (ptr != NULL && ptr->key == ST_LIST)
                  ptr = push_right_go_left (ptr, S); 
            //LOCAL_VAR 
            if (ptr != NULL && ptr->key == LOCAL_VAR){
                  ptr = push_right_go_left (ptr, S);
                  //DECLARATION
                  ptr = push_right_go_left (ptr, S);
                  //Type
                  load_type (ptr->key, &typ); //load typ of var {I,D,S} to typ
                  if (typ == 'V'){
                        DStack (S);
                        return 2;                  
                  }
                  types = add_type_before_types ('P', &typ);
                  if (types == NULL){
                        DStack (S);      
                        return 99;
                  }
                  ptr = STopPop (S);
                  //ID
                  name = ptr->literal;

                  error = check_exist_function(HTable, name);
                  if (error != 0){
                        DStack (S);
                        return error;
                  }

                  if (IAL_htSearch(LHTable, name) == NULL){
                        error = IAL_htInsert(LHTable, name, 0, types);
                        free(types);
                        if (error != 0){
                              DStack (S);
                              return 99;
                        }
                  }
                  else{
                        free(types);
                        DStack (S);
                        return 3;      
                  }
                 
     
                  //EXPRESSION
                  if (!SEmpty (S) && STop(S)->key == EXPRESSION){
                        ptr = STopPop (S);
                        error = expression_control(ptr, HTable, LHTable, typ, 0);
                        if (error != 0){
                              DStack (S);      
                              return error;      
                        }
                  }

            }
            //STATEMENT
            if (ptr != NULL && ptr->key == STATEMENT){
                  error = statement_control(ptr, HTable, LHTable, ftypes);
                  if (error != 0){
                        DStack (S);
                        return error;
                  }
                  
            }
              
      }while (!SEmpty (S));
            
      return 0;
}

/* Control statement of function */
int statement_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      char *name;
      char typ;
      char returns;

      SInit (S);

      //STATEMENT
      ptr = ptr->LPtr;
      //ASSIGNMENT
      if (ptr->key == ASSIGNMENT){
            ptr = push_right_go_left (ptr, S);
            //ID
            name = ptr->literal;
            //Try find variable
            if (LHTable != NULL)//Try find local
                  item = IAL_htSearch(LHTable, name);
            else 
                  item = NULL;
                  
            if (item == NULL){//Try find global
                  error = try_find_global_load_item(ptr, HTable, name);
                  if (error != 0){
                        DStack (S);
                        return error;
                  }
            }

            typ = item->types[1];
            ptr = STopPop (S);
            //EXPRESSION
            error = expression_control(ptr, HTable, LHTable, typ, 0);
            if (error != 0){
                  DStack (S);      
                  return error;      
            }
      }

      //CONDITION (if statement)
      if (ptr->key == CONDITION){ 
            ptr = push_right_go_left (ptr, S);
            //COMPARISON
            if (ptr->literal == NULL){//check emty condition
                  DStack (S);
                  return 4;
            }
            ptr = push_right_go_left (ptr, S);
            //EXPRESSION
            if (ptr != NULL){
                  error = expression_control(ptr, HTable, LHTable, 'D', 0);
                  if (error != 0){
                        DStack (S);
                        return error;
                  }            
            }
            if (STop (S)->key == EXPRESSION){
                  ptr = STopPop (S);
                  //EXPRESSION
                  error = expression_control(ptr, HTable, LHTable, 'D', 0);
                  if (error != 0){
                        DStack (S);
                        return error;
                  } 
            }
            ptr = STopPop (S);
            //CONDITION 2
            if (ptr->LPtr != NULL){
                  //BLOCK_LIST
                  error = block_list_control(ptr->LPtr, HTable, LHTable, ftypes);
                  if (error != 0){
                        DStack (S);
                        return error;
                  }      
      
                  //BLOCK_LIST
                  if (ptr->RPtr != NULL){
                        error = block_list_control(ptr->RPtr, HTable, LHTable, ftypes);
                        if (error != 0){
                              DStack (S);
                              return error;
                        }
                  }
            }       
      }
      //CYCLE (while)
      if (ptr->key == CYCLE){
            ptr = push_right_go_left (ptr, S);
            //COMPARISON
            if (ptr->literal == NULL){//check emty condition
                  DStack (S);
                  return 4;
            }
            ptr = push_right_go_left (ptr, S);
            //EXPRESSION
            if (ptr != NULL){
                  error = expression_control(ptr, HTable, LHTable, 'D', 0);
                  if (error != 0){
                        DStack (S);
                        return error;
                  }      
            }
            if (STop (S)->key == EXPRESSION){
                  ptr = STopPop (S);
                  //EXPRESSION
                  error = expression_control(ptr, HTable, LHTable, 'D', 0);
                  if (error != 0){
                        DStack (S);
                        return error;
                  } 
            }
            ptr = STopPop (S);
            //BLOCK_LIST
            error = block_list_control(ptr, HTable, LHTable, ftypes);
            if (error != 0){
                  DStack (S);
                  return error;
            }
      }

      //CALL (call function)
      if (ptr->key == CALL){
            error = call_control(ptr, HTable, LHTable, &returns);
            if (error != 0){
                  DStack (S);      
                  return error;      
            }
      }

      //RETURN (return statement)
      if (ptr->key == RETURN){
            ptr = ptr->LPtr;
            if (ptr != NULL){
                  if (ftypes[1] == 'V'){
                        DStack(S);
                        return 4;
                  }
                  error = expression_control(ptr, HTable, LHTable, ftypes[1], 0);
                  if (error != 0){
                        DStack (S);      
                        return error;      
                  }         
            }
      } 
      return 0;               
}

/* Call statement_control for statement */
int block_list_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      SInit (S);
      SPush (S, ptr);

      do{
            ptr = STopPop (S);
            //BLOCK_LIST
            ptr = push_right_go_left (ptr, S);
            //STATEMENT
            if (ptr != NULL){
                  error = statement_control(ptr, HTable, LHTable, ftypes);
                  if (error != 0){
                        DStack (S);      
                        return error;      
                  }      
            }
            
      }while(!SEmpty (S));

      return 0;
}

/* Control call function, control exist function, control true arguments */
int call_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *returns) {
      char *fullname;
      char *ftypes;      
      char *name;
      int fspecial = 0;
      int i = 2;
      int max;
      tStackPtr stack;      
      tStackPtr *S = &stack;

      SInit (S);
      //CALL 
      ptr = push_right_go_left (ptr, S);
      //ID      
      name = ptr->literal;
      if (strcmp("ifj16.print",name) == 0)
            fspecial = -1;

      //Try find function
      error = try_find_global_load_item(ptr, HTable, name);
      if (error != 0){
            DStack (S);
            return error;
      }

      if ( (item->types)[0] != 'F'){
            DStack (S);
            return 3;
      } 
      ftypes = item->types;
      max = item->n;
      *returns = ftypes[1];
      
      do{
            ptr = STopPop (S);
            //PAR_LIST
            if (ptr != NULL && ptr->key == PAR_LIST)          
                  ptr = push_right_go_left (ptr, S);
            //ID
            if (ptr != NULL && ptr->key == ID){
                  name = ptr->literal;
                  //Try find variable
                  if (LHTable != NULL)//Try find local
                        item = IAL_htSearch(LHTable, name);
                  else
                        item = NULL;
                  
                  if (item == NULL){//Try find global
                        error = try_find_global_load_item(ptr, HTable, name);
                        if (error != 0){
                              DStack (S);
                              return error;
                        }
                  }
                  
                  if ( (item->types)[0] != 'P'){
                        DStack (S);
                        return 3;
                  }

                  if (fspecial == 0){
                        //Control param type    
                        if (i > max || ftypes[i] != item->types[1]){
                              DStack (S);
                              return 4;
                        }
                  }
                  i++;
            }

            //EXPRESSION
            if (ptr != NULL && ptr->key == EXPRESSION){
                  if (i > max){
                        DStack(S);
                        return 4;
                  }
                  error = expression_control(ptr, HTable, LHTable, ftypes[i], fspecial);
 
                  if (error != 0){
                        DStack (S);                      
                        return error;
                  }
                  i++;       
            }

      }while (!SEmpty (S));
 
      if (fspecial == 0){
            if (ftypes[i] != '\0'){
                  return 4;
            }
      }
      else{
            if (i == 2){
                  return 4;
            }
      }    

        return 0;
}

/* Control expression */
//param special < 0 - ifj16.print, special > 0 - rate of static var
int expression_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char typ , int special){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      tStackPtr stack2;      
      tStackPtr *S2 = &stack2;
      tStackPtr stack3;      
      tStackPtr *S3 = &stack3;
      char *name;
      char tvar; 
      char *types = NULL;
      char *help = NULL;
      int term = 0;
      int count = 0;
      
      SInit (S);
      SInit (S2);
      SInit (S3);
      SPush (S, ptr);
      
      do{
            ptr = STopPop (S);
            //EXPRESSION  
            if (ptr != NULL){
                  while (ptr->key == EXPRESSION){    
                        ptr = push_right_go_left (ptr, S);   
                  }           
             }
                    
            //TERM
            if (ptr != NULL && ptr->key == TERM){
                  term = 1;
                  if (ptr->literal != NULL){
                        SPush (S3, ptr);   
                  }
                  if (ptr->RPtr != NULL) 
                        SPush (S, ptr->RPtr);
                  if (ptr->LPtr != NULL) 
                        SPush (S, ptr->LPtr);
            }

            //CALL
            if (ptr != NULL && ptr->key == CALL){
                  //Call in inicialization static var
                  if (special > 0){
                        free(types);  
                        DStack(S);
                        DStack(S2);
                        return 6;
                  }
                  error = call_control(ptr, HTable, LHTable, &tvar);
                  if(error != 0){
                        free(types);
                        DStack (S);
                        DStack(S2);
                        return error;
                  }

                  help = add_type_before_types (tvar, types);
                  free(types);
                  types = help; 

                  //Check return typ of function for S  
                  if (typ == 'S'){
                        SPush (S2, ptr);
                        if (tvar == 'V'){
                              free(types);
                              DStack (S);
                              DStack(S2);
                              return 8;
                        }      
                        if (term > 0 && tvar == 'S'){ 
                              free(types);                                
                              DStack (S);
                              DStack(S2);
                              return 4;
                        }                         
                  }
                  else{     
                        //Check return typ of function for other            
                        if (tvar != typ){
                              if (typ == 'D' && tvar == 'I'){
                                    if (term == 0 || typ != 'D'){      
                                          error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                                          if (error != 0){
                                                DStack (S);
                                                DStack(S2);
                                                return error;
                                          }
                                    }
                                    else{
                                          SPush (S3, ptr);    
                                          count++;  
                                    }
                              }
                              else{     
                                    DStack (S);
                                    DStack(S2);
                                    //Bad type return
                                    if (tvar != 'V'){
                                          return 4;
                                    }
                                    //Try get return from void
                                    else{
                                          return 8;     
                                    }
                              }
                        }    
                  } 
                  term = 0;              
            }

            //ID
            if (ptr != NULL && ptr->key == ID){
                  name = ptr->literal;
                  //Try find variable
                  if (LHTable != NULL){//Try find local
                        item = IAL_htSearch(LHTable, name);
                  }
                  else{
                        item = NULL;
                  }

                  if (item == NULL){//Try find global
                        error = try_find_global_load_item(ptr, HTable, name);
                        if (error != 0){
                              free(types);
                              DStack (S);
                              DStack(S2);
                              return error;
                        }

                        //Static var in inicialization static var, error for later declaration static param in class
                        if (special > 0){
                              name = add_class_before_name (ActClass, "\0");
                              if (strstr(item->id, name) != NULL){
                                    if (item->index >= special){
                                          free(types);
                                          free(name);
                                          DStack(S);
                                          DStack(S2);
                                          return 6;
                                    }
                              }
                              free(name);
                        }
                  }
                  
                  if ( (item->types)[0] != 'P'){
                        free(types);
                        DStack (S);
                        DStack(S2);
                        return 3;
                  }
                        
                  tvar = (item->types)[1];
                  help = add_type_before_types (tvar, types);
                  free(types);
                  types = help; 
                  //Check type for String
                  if (typ == 'S'){
                        SPush (S2, ptr);
                        if (term > 0 && tvar == 'S'){ 
                              free(types);                               
                              DStack (S);
                              DStack(S2);
                              return 4;
                        }                               
                  }
                  else{
                        //Check type for other
                        if (tvar != typ){
                              if (typ == 'D' && tvar == 'I'){
                                    if (term == 0 || typ != 'D'){      
                                          error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                                          if (error != 0){
                                                DStack (S);
                                                DStack(S2);
                                                return error;
                                          }
                                    }
                                    else{
                                          SPush (S3, ptr);    
                                          count++;  
                                    }
                              }
                              else{
                                    DStack (S);
                                    DStack(S2);
                                    return 4;
                              }       
                        }
                  }
                  term = 0;
            }          
                            
            //LITERAL
            if (ptr != NULL && (ptr->key == STRING || ptr->key == INT || ptr->key == DOUBLE) ){
                  load_type_literal (ptr->key, &tvar);

                  help = add_type_before_types (tvar, types);
                  free(types);
                  types = help; 
                  //Check type for String
                  if (typ == 'S'){
                        SPush (S2, ptr);
                        if (term > 0 && tvar == 'S'){ 
                              free(types);                              
                              DStack (S);
                              DStack(S2);
                              return 4;
                        }                             
                  }
                  else{
                        //Check type for other
                        if (tvar != typ){
                              if (typ == 'D' && tvar == 'I'){
                                    if (term == 0 || typ != 'D'){      
                                          error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                                          if (error != 0){
                                                DStack (S);
                                                DStack(S2);
                                                return error;
                                          }
                                    }
                                    else{
                                          SPush (S3, ptr);    
                                          count++;  
                                    }
                              }
                              else{
                                    DStack (S);
                                    DStack(S2);
                                    return 4;
                              }  
                        }    
                  }   
                  term = 0;                   
            }

      if (count == 2){
            error = double_control(S3, &types);
            count = 0;
            DStack (S3);
            if (error != 0){
                  DStack (S);
                  DStack(S2);
                  return error;
            }
            
      }

      }while (!SEmpty (S));
           
      
      if (typ == 'S'){
            error = string_control(S2, types, special);
            DStack(S2);
            free(types);
            return (error != 0)? error : 0;
      }
      else{
            DStack(S2);
            return 0;
      }
      
                
}

/* double term control */
int double_control(tStackPtr *S, char **type){
      tTNodePtr ptr = NULL;
      char *types = *type;

      if (types[0] == 'I' && types[1] == 'I'){
            ptr = STopPop (S);
            ptr = STopPop (S);
            ptr = STopPop (S); 
            types[0] = 'D';
            types[1] = 'D';
            error = subtree_int_to_real(&ptr); //repleace int to int_to_real
            return (error == 0)? 0 : error;     
      }

      if (types[0] == 'D' && types[1] == 'I'){
            ptr = STopPop (S);
            ptr = STopPop (S);
            types[1] = 'D';
            error = subtree_int_to_real(&ptr); //repleace int to int_to_real
            return (error == 0)? 0 : error;     
      }

      if (types[0] == 'I' && types[1] == 'D'){
            ptr = STopPop (S);
            types[0] = 'D';
            error = subtree_int_to_real(&ptr); //repleace int to int_to_real
            return (error == 0)? 0 : error;     
      }

      return 0;
}

/* string concationation control */
int string_control(tStackPtr *S, char *types, int special){
      int found_string = 0;
      int found_double = 0;
      int max = 0;
      tTNodePtr ptr;

      //find string
      if(strstr(types, "S") != NULL){
            found_string = 1;
      }

      //find double
      if(strstr(types, "D") != NULL){
            found_double = 1;
      }

      while(types[max] != '\0'){
            max++;
      }

      if (found_double == 1){
            for (int i=0; i<(max); i++){
                  ptr = STopPop (S);
                  if (types[i] == 'I'){
                        error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                        if (error != 0){
                              return error;
                        }      
                  }
            }
      }

      if (special < 0){
            return 0;
      }

      return (found_string)? 0 : 4;
}

/*try find param name global and load global pointer to HT item */
static inline int try_find_global_load_item(tTNodePtr ptr, IAL_HashTable *HTable, char *name){
      char *fullname;
      
       item = IAL_htSearch(HTable, name);
       if (item == NULL){
            fullname = add_class_before_name (ActClass, name);
            if (error != 0){                              
                return (error == 99) ? 99 : 2;
            }                        
            item = IAL_htSearch(HTable, fullname);
            if (item == NULL){
                  free(fullname);                     
                  return 3;  
            }    
            else{ //full id to tree
                  free(ptr->literal);
                  ptr->literal = fullname; 
            }      
      }

      return 0;
}

/*load type for literal*/
static inline void load_type_literal (node_t key, char *typ){
      if (key == STRING)
            *typ = 'S';
      else
            if (key == INT)
                  *typ = 'I';
            else
                  *typ = 'D';                                     
}

/*load inner functions to HT*/
int load_inner (IAL_HashTable *HTable){

      error = load_inner_function (HTable, "ifj16.readInt", "FI");
      if (error != 0)
            return 99;       
      error = load_inner_function (HTable, "ifj16.readDouble", "FD");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.readString", "FS");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.print", "FVS");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.length", "FIS");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.substr", "FSSII");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.compare", "FISS");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.find", "FISS");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.sort", "FSS");
      if (error != 0)
            return 99;
      error = load_inner_function (HTable, "ifj16.IntToReal", "FDI");
      if (error != 0)
            return 99;

      return 0;
}

/* helping function for load inner functions */
int load_inner_function (IAL_HashTable *HTable, char *id, char *t){
      char *name;
      char *types;
      size_t size;

      size = strlen (id) + 1;
      name = malloc(size); 
      name = strcpy(name, id);
      
      size = strlen (t) + 1;
      types = malloc(size);
      types = strcpy(types, t);

      error = IAL_htInsert(HTable, name, 0, types);
      free(name);
      free(types);
      
      if (error != 0)
            return error;     
      else
            return 0;
}

/* first going throught tree, load static var and static functional to HT */
int load_static (tTNodePtr ptr, IAL_HashTable *HTable){      
      tStackPtr stack;      
      tStackPtr *S = &stack;
      IAL_HashTable Table;
      IAL_HashTable *CHTable = &Table; //Class table
 
      IAL_htInit(CHTable);
      IAL_htInsert(CHTable, "ifj16", 0, "C"); //not redefinition ifj16 class
      SInit (S);
      SPush (S, ptr);
      do{
            ptr = STopPop (S);
            //CLASS_LIST
            if (ptr != NULL && ptr->key == CLASS_LIST){
                  ptr = push_right_go_left (ptr, S);
            }
            
            //CLASS
            if (ptr != NULL && ptr->key == CLASS){
                  ptr = push_right_go_left (ptr, S);
                  ActClass = ptr->literal;  
                  if (error = IAL_htInsert(CHTable, ActClass, 0, "C") != 0){ //Control class redefinition
                        DStack (S);
                        IAL_htDispose(CHTable);
                        return (error == 99)? 99 : 3;
                  }
                  continue;            
            }
            //CLASS_ITEM
            if (ptr != NULL && ptr->key == CLASS_ITEM){
                  if (ptr->LPtr != NULL)
                        ptr = push_right_go_left (ptr, S);
            }
            //STATIC_VAR
            if (ptr != NULL && ptr->key == STATIC_VAR){
                  error = static_var_htinsert(&ptr, S, HTable);
                  if (error != 0){
                        IAL_htDispose(CHTable);
                        DStack (S);
                        return error;
                  }      
            }
            //FUNCTION
            if (ptr != NULL && ptr->key == FUNCTION){  
                  error = function_htinsert(&ptr, S, HTable);
                  if (error != 0){
                        IAL_htDispose(CHTable);
                        DStack (S);
                        return error;
                  }                        
            }
      }while (!SEmpty (S));
      
      IAL_htDispose(CHTable);
      //Check exist Main.run in file
      item = IAL_htSearch(HTable, "Main.run");
      if (item == NULL)
            return 3;
      
      //Bad def run
      if (strcmp(item->types,"FV\0") != 0)
            return 4;
      
      return 0;     
}      

/*helping function for load static functional to HT*/
int function_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable){
      char typ;
      char *types = NULL;
      char *name = NULL; 
      char *help = NULL;     
      tTNodePtr ptr;
   
      ptr = *original;
      SPush (S, ptr->RPtr);
      ptr = ptr->LPtr;
      //DECLARATION
      SPush (S, ptr->RPtr);      
      ptr = ptr->LPtr;
      //Typ
      load_type (ptr->key, &typ);
      types = add_type_before_types ('F', &typ);
      ptr = STopPop (S);
      //ID
      name = ptr->literal;
      name = add_class_before_name (ActClass, name);
      if (name == NULL){
            free(types);
            DStack (S);      
            return (error == 99) ? 99 : 2;       
      }
      ptr = STopPop (S);
      //FUNCTION 2
      ptr = ptr->LPtr;
      //ARG_LIST
      SPush (S, ptr);
      while(SEmpty (S) != 1 && (STop(S))->key == ARG_LIST){ //Load params to types
            ptr = STopPop (S);
            if (ptr->LPtr != NULL){
                  ptr = push_right_go_left (ptr, S);
                  //DECLARATION
                  ptr = ptr->LPtr;
                  //TYP
                  load_type (ptr->key, &typ);
                  help = add_char_behind_types (types, typ);
                  free(types);
                  if (help == NULL){
                        free(name);
                        DStack (S);      
                        return 99;                             
                  }
                  types = help;           
            }
      }
      if (IAL_htSearch(HTable, name) == NULL){
            error = IAL_htInsert(HTable, name, 0, types);
            free(name);
            free(types);
            if (error != 0){
                  DStack (S);
                  return 99;
            }           
      }
      else{
            free(name);
            free(types);
            DStack (S);
            return 3;      
      }

      *original = ptr;
      return 0;
         
}

/*helping function for load static var to HT*/
int static_var_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable){
      char typ;
      char *types = NULL;
      char *name = NULL;
      int i = 1;    
      tTNodePtr ptr; 

      ptr = *original;
      ptr = ptr->LPtr;
      //DECLARATION      
      SPush (S, ptr->RPtr);      
      ptr = ptr->LPtr;
      //Type
      load_type (ptr->key, &typ); //load typ of var {I,D,S} to typ
      if (typ == 'V'){
            free(types);
            DStack (S);
            return 2;                  
      }
      types = add_type_before_types ('P', &typ);
      if (types == NULL){
            DStack (S);      
            return 99;
      }
      ptr = STopPop (S);
      //ID
      name = ptr->literal;
      name = add_class_before_name (ActClass, name);
      if (name == NULL){
            free(types);
            DStack (S);      
            return (error == 99) ? 99 : 2;       
      }
      if (IAL_htSearch(HTable, name) == NULL){
            error = IAL_htInsert(HTable, name, i++, types);
            free(name);
            free(types);           
            if (error != 0){
                  DStack (S);
                  return 99;
            }
      }
      else{
            free(name);
            free(types);
            DStack (S);
            return 3;      
      }
      *original = ptr;
      return 0; 
}

/*helping function push right to stack go left */
static inline tTNodePtr push_right_go_left (tTNodePtr ptr, tStackPtr *S){
       if (ptr->RPtr != NULL)
            SPush (S, ptr->RPtr);
       return ptr->LPtr;
}

/*helping function add class before name*/
static inline char* add_class_before_name (char *ActClass, char *name){
      size_t size;
      char* new;

      if (strchr(name, (int)'.') != NULL){
            return NULL;      
      }

      size = strlen(ActClass) + strlen(name) + 2;
      new = malloc (size);
      if (new != NULL){
            new = strcpy(new, ActClass);
            new = strcat(new, ".");
            new = strcat(new, name);
      }
      else
            error = 99;

      return new;          
}

/*helping function load type*/
static inline void load_type (node_t key, char *typ){
      if (key == STRING_DATA)
            *typ = 'S';
      else{
            if (key == INT_DATA)
                  *typ = 'I';
            else
                  if (key == DOUBLE_DATA)
                        *typ = 'D';
                  else                  
                        *typ = 'V'; 
      }                       
}

/*helping function add char before string*/
static inline char* add_type_before_types (char typ, char *types){
      size_t size;
      char* new;
      
      if (types != NULL){
            size = strlen(types) + 2;
      }
      else{
            size = 2;
      }
      
      new = malloc (size);
      if (new != NULL){
            new[0] = typ;
            new[1] = '\0';
            if(types != NULL){
                  new = strcat(new, types);
            }      
      }
      return new;
}

/*helping function add char behind string*/
static inline char *add_char_behind_types (char *types, char c){
      size_t size;
      char* new;
      
      if (types != NULL){
            size = strlen(types);
      }      
      else{
            size = 0;      
      }

      new = malloc (size + 2);
      if (new != NULL){
            if (types != NULL){
                  new = strcpy(new, types);
            }
            new[size] = c;
            new[size+1] = '\0';                 
      }

      return new;
}

