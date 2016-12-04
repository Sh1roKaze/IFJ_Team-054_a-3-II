// sematic.c
// 22. 11. 2016
// (c) Jan Oškera
// V rámci projektu do předmětů IFJ a IAL

#include "sematics.h"

int DetectStackError;
static char *ActClass;
static int error = 0;
static IAL_htItem *item = NULL;

//Prototypes
int subtree_int_to_real(tTNodePtr* ptr);
int sematics(tTNodePtr ptr, IAL_HashTable *Table);
int function_control(tTNodePtr ptr, IAL_HashTable *HTable);
int st_list_control (tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes);
int statement_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes);
int block_list_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *ftypes);
int call_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *returns);
int expression_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char typ);
int print_expression_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable);
int load_inner (IAL_HashTable *HTable);
int load_inner_function (IAL_HashTable *HTable, char *id, char *t);
int load_static (tTNodePtr ptr, IAL_HashTable *HTable);
int function_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable);
int static_var_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable);
static inline int try_find_global_load_item(tTNodePtr ptr, IAL_HashTable *HTable, char *name);
static inline void load_typ_literal (node_t key, char *typ);
static inline char* add_class_before_name (char *ActClass, char *name);
static inline void load_typ (node_t key, char *typ);
static inline char* add_typ_before_types (char typ, char *types);
static inline char *add_char_behind_types (char *types, char c);
static inline int check_exist_function(IAL_HashTable *HTable, char *name);
static inline tTNodePtr push_right_go_left (tTNodePtr ptr, tStackPtr *S);

//build structure for call function int to real, repleace int in tree
int subtree_int_to_real(tTNodePtr* ptr){
      tTNodePtr id_f = malloc (sizeof(struct tTNode));
      tTNodePtr par_list = malloc (sizeof(struct tTNode));
      tTNodePtr id_p = malloc (sizeof(struct tTNode));
      
      if (id_f != NULL && par_list != NULL && id_p != NULL){        
            id_f->LPtr = NULL;
            id_f->RPtr = NULL;
            id_f->key = ID;
            id_f->literal = malloc((strlen("IFJ16.IntToReal")+1)*sizeof(char));
            strcpy(id_f->literal,"IFJ16.IntToReal");
            
            par_list->LPtr = id_p;
            par_list->RPtr = NULL;
            par_list->key = PAR_LIST;

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

int sematics(tTNodePtr ptr, IAL_HashTable *HTable){
      tStackPtr stack;      
      tStackPtr *S = &stack;      
      char typ;

      error = load_inner (HTable);
      if (error != 0){
            return error;                  
      }

      error = load_static (ptr, HTable);
      if (error != 0){
            return error;                  
      }
      
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
                  continue;          
            }
            //CLASS_ITEM
            if (ptr != NULL && ptr->key == CLASS_ITEM){
                  if (ptr->LPtr != NULL)
                        ptr = push_right_go_left (ptr, S);  
            }
            //STATIC_VAR
            if (ptr != NULL && ptr->key == STATIC_VAR){
                  ptr = push_right_go_left (ptr, S);
                  //DECLARATION
                  ptr = ptr->LPtr;
                  //TYP
                  load_typ (ptr->key, &typ);
                  if ( SEmpty (S) != 1 && STop(S)->key == EXPRESSION ){
                        ptr = STopPop (S);
                        //EXPRESSION
                        error = expression_control(ptr, HTable, NULL, typ); 
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

static inline int check_exist_function(IAL_HashTable *HTable, char *name){
      char *fullname;
      
      fullname = add_class_before_name (ActClass, name);
      if (fullname == NULL){
            return (error == 99)? 99 : 3;
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
            return 99;       
      }
      item = IAL_htSearch(HTable, name);
      free(name);      
      types = item->types;
      ptr = STopPop (S);
      //FUNCTION 2
      ptr = push_right_go_left (ptr, S);
      //ARG_LIST
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
                  load_typ (ptr->key, &typ); //load typ of var {I,D,S} to typ
                  if (typ == 'V'){
                        DStack (S);
                        return 6;                  
                  }
                  types = add_typ_before_types ('P', &typ);
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
                        error = expression_control(ptr, HTable, LHTable, typ);
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
            error = expression_control(ptr, HTable, LHTable, typ);
            if (error != 0){
                  DStack (S);      
                  return error;      
            }
      }

      //CONDITION
      if (ptr->key == CONDITION){ //if statement
            ptr = push_right_go_left (ptr, S);
            //COMPARISON
            if (ptr->literal == NULL){//check emty condition
                  DStack (S);
                  return 4;
            }
            ptr = push_right_go_left (ptr, S);
            //EXPRESSION
            if (ptr != NULL){
                  error = expression_control(ptr, HTable, LHTable, 'D');
                  if (error != 0){
                        DStack (S);
                        return error;
                  }            
            }
            if (STop (S)->key == EXPRESSION){
                  ptr = STopPop (S);
                  //EXPRESSION
                  error = expression_control(ptr, HTable, LHTable, 'D');
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
      //CYCLE
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
                  error = expression_control(ptr, HTable, LHTable, 'D');
                  if (error != 0){
                        DStack (S);
                        return error;
                  }      
            }
            if (STop (S)->key == EXPRESSION){
                  ptr = STopPop (S);
                  //EXPRESSION
                  error = expression_control(ptr, HTable, LHTable, 'D');
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

      //CALL
      if (ptr->key == CALL){
            error = call_control(ptr, HTable, LHTable, &returns);
            if (error != 0){
                  DStack (S);      
                  return error;      
            }
      }

      //RETURN
      if (ptr->key == RETURN){
            ptr = ptr->LPtr;
            if (ptr != NULL){
                  error = expression_control(ptr, HTable, LHTable, ftypes[1]);
                  if (error != 0){
                        DStack (S);      
                        return error;      
                  }         
            }
      } 
      return 0;               
}

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

int call_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char *returns) {
      char *fullname;
      char *ftypes;      
      char *name;
      int fspecial = 0;
      int i = 2;
      tStackPtr stack;      
      tStackPtr *S = &stack;

      SInit (S);
      //CALL 
      ptr = push_right_go_left (ptr, S);
      //ID      
      name = ptr->literal;
      if (strcmp("ifj16.print",name) == 0)
            fspecial = 1;

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

                  if (!fspecial){
                        //Control param type                  
                        if (ftypes[i] != item->types[1]){
                              DStack (S);
                              return 4;
                        }
                  }
                  i++;
            }

            //EXPRESSION
            if (ptr != NULL && ptr->key == EXPRESSION){
                  if (!fspecial){
                        error = expression_control(ptr, HTable, LHTable, ftypes[i]);
                  }
                  else{
                        error = print_expression_control(ptr, HTable, LHTable);
                  }

                  if (error != 0){
                        DStack (S);                        
                        return error;
                  }
                  i++;       
            }

      }while (!SEmpty (S));
 
      if (!fspecial){
            if (ftypes[i] != '\0')
                  return 4;
      }
      else{
            if (i == 2)
                  return 4;
      }    

        return 0;
}

int print_expression_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      char returns;
      char *name;
      
      SInit (S);
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
            if (ptr != NULL && ptr->key == TERM)          
                  ptr = push_right_go_left (ptr, S);

            //CALL
            if (ptr != NULL && ptr->key == CALL){
                  error = call_control(ptr, HTable, LHTable, &returns);
                  if(error != 0){
                        DStack (S);
                        return error;
                  }                  
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
                              DStack (S);
                              return error;
                        }
                  }
                  
                  if ( (item->types)[0] != 'P'){
                        DStack (S);
                        return 3;
                  }      
            }          

      }while (!SEmpty (S));

      return 0;        
}

int expression_control(tTNodePtr ptr, IAL_HashTable *HTable, IAL_HashTable *LHTable, char typ){
      tStackPtr stack;      
      tStackPtr *S = &stack;
      char returns;
      char *name;
      char *fullname;
      char tvar; 
      
      SInit (S);
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
            if (ptr != NULL && ptr->key == TERM)          
                  ptr = push_right_go_left (ptr, S);

            //CALL
            if (ptr != NULL && ptr->key == CALL){
                  error = call_control(ptr, HTable, LHTable, &returns);
                  if(error != 0){
                        DStack (S);
                        return error;
                  }

                  //Check return typ of function                  
                  if (returns != typ){
                        if (typ == 'D' && returns == 'I'){
                              error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                              if (error != 0){
                                    DStack (S);
                                    return error;
                              }
                        }
                        else{
                            
                              if (returns != 'V'){
                                    DStack (S);
                                    return 4;
                              }
                        }
                  }                   
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
                              DStack (S);
                              return error;
                        }
                  }
                  
                  if ( (item->types)[0] != 'P'){
                        DStack (S);
                        return 3;
                  }      
                  tvar = (item->types)[1];
                  if (tvar != typ){
                        if (typ == 'D' && tvar == 'I'){
                              error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                              if (error != 0){
                                    DStack (S);
                                    return error;
                              }
                        }
                        else{
                              DStack (S);
                              return 4;
                        }       
                  }
            }          
                            
            //LITERAL
            if (ptr != NULL && (ptr->key == STRING || ptr->key == INT || ptr->key == DOUBLE) ){
                  load_typ_literal (ptr->key, &tvar);
                  
                  if (tvar != typ){
                        if (typ == 'D' && tvar == 'I'){
                              error = subtree_int_to_real(&ptr); //repleace int to int_to_real
                              if (error != 0){
                                    DStack (S);
                                    return error;
                              }
                        }
                        else{
                              DStack (S);
                              return 4;
                        }  
                  }                          
            }

      }while (!SEmpty (S));

      return 0;            
}

static inline int try_find_global_load_item(tTNodePtr ptr, IAL_HashTable *HTable, char *name){
      char *fullname;
      
       item = IAL_htSearch(HTable, name);
       if (item == NULL){
            fullname = add_class_before_name (ActClass, name);
            if (error != 0){                              
                return error;
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

static inline void load_typ_literal (node_t key, char *typ){
      if (key == STRING)
            *typ = 'S';
      else
            if (key == INT)
                  *typ = 'I';
            else
                  *typ = 'D';                                     
}

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
      return (IAL_htSearch(HTable, "Main.run") != NULL)? 0 : 3;     
}      

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
      load_typ (ptr->key, &typ);
      types = add_typ_before_types ('F', &typ);
      ptr = STopPop (S);
      //ID
      name = ptr->literal;
      name = add_class_before_name (ActClass, name);
      if (name == NULL){
            free(types);
            DStack (S);      
            return (error == 99) ? 99 : 3;       
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
                  load_typ (ptr->key, &typ);
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

int static_var_htinsert(tTNodePtr *original, tStackPtr *S, IAL_HashTable *HTable){
      char typ;
      char *types = NULL;
      char *name = NULL;
      int i = 0;    
      tTNodePtr ptr; 

      ptr = *original;
      ptr = ptr->LPtr;
      //DECLARATION      
      SPush (S, ptr->RPtr);      
      ptr = ptr->LPtr;
      //Type
      load_typ (ptr->key, &typ); //load typ of var {I,D,S} to typ
      if (typ == 'V'){
            free(types);
            DStack (S);
            return 6;                  
      }
      types = add_typ_before_types ('P', &typ);
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
            return (error == 99) ? 99 : 3;       
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


static inline tTNodePtr push_right_go_left (tTNodePtr ptr, tStackPtr *S){
       if (ptr->RPtr != NULL)
            SPush (S, ptr->RPtr);
       return ptr->LPtr;
}

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

static inline void load_typ (node_t key, char *typ){
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

static inline char* add_typ_before_types (char typ, char *types){
      size_t size;
      char* new;
      
      size = strlen(types) + 2;
      new = malloc (size);
      if (new != NULL){
            new[0] = typ;
            new[1] = '\0';
            new = strcat(new, types);
      }
      return new;
}

static inline char *add_char_behind_types (char *types, char c){
      size_t size;
      char* new;
      
      size = strlen(types);
      new = malloc (size + 2);
      if (new != NULL){
            new = strcpy(new, types);
            new[size] = c;
            new[size+1] = '\0';                 
      }

      return new;
}

