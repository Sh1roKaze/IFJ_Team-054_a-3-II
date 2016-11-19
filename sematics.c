#include "sematics.h"

int DetectStackError;

//build structure for call function int to real, repleace int in tree
int subtree_int_to_real(tTNodePtr* ptr){
      tTNodePtr id_f = malloc (struct tTNode);
      tTNodePtr par_list = malloc (struct tTNode);
      tTNodePtr param = malloc (struct tTNode);
      tTNodePtr id_p = malloc (struct tTNode);
      
      if (id_f != NULL && par_list != NULL && param != NULL && id_p != NULL){        
            id_f->LPtr = NULL;
            id_f->RPtr = NULL;
            id_f->key = IDENTIFIER;
            id_f->literal = "IntToReal";
            
            par_list->LPtr = param;
            par_list->RPtr = NULL;
            par_list->key = PAR_LIST;
                  
            param->LPtr = id_p;
            param->RPtr = NULL;
            param->key = PAR_LIST;

            id_p->LPtr = NULL;
            id_p->RPtr = NULL;
            id_p->key = IDENTIFIER;
            id_p->literal = (*ptr)->literal;
            
            (*ptr)->LPtr = id_f;
            (*ptr)->RPtr = par_list;
            (*ptr)->key = CALL;
            (*ptr)->literal = "";
                  
      }
      else{
            free(id_f);
            free(par_list);
            free(param);
            free(id_p);
            return(99);
      }

int sematics(tTNodePtr ptr){
      int returns = 0;
      if (ptr != NULL){
            parsHT_Table *SHTable = malloc(sizeof(parsHT_Table));  //check if tonda want this    
            if (SHTable == NULL)
                  return 99;

            parsHT_Init(SHTable);
            returns = load_static (ptr, SHTable);
            if (returns != 0){
                  free(SHTable);
                  return returns;                  
            }
       
      }
      else
            return 0;// maybe -1 or delete
}

int 

int load_static (tTNodePtr ptr, parsHT_Table *HTable){      
      tStackPtr *S;
      char *ActClass = NULL;
      int error = 0;

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
                  ptr = STopPop (S);            
            }
            //CLASS_ITEM
            if (ptr != NULL && ptr->key == CLASS_ITEM){
                  ptr = push_right_go_left (ptr, S);
            }
            //STATIC_VAR
            if (ptr != NULL && ptr->key == STATIC_VAR){
                  error = static_var_htinsert(&ptr, S, ActClass, HTable);
                  if (error != 0)
                        return error;      
            }
            //FUNCTION
            if (ptr != NULL && ptr->key == FUNCTION){  
                  error = function_htinsert(&ptr, S, ActClass, HTable);
                  if (error != 0)
                        return error;                        
            }
      }while (SEmpty (S));
      
      return 0;     
}      

int function_htinsert(tTNodePtr *original, tStackPtr *S, char *ActClass, parsHT_Table *HTable){
      char typ = '';
      char *typs = NULL;
      char *name = NULL; 
      char *help = NULL;    
      int error = 0;  
      tTNodePtr ptr;
      
      ptr = *original;
      SPush (S, ptr->RPtr);
      ptr = ptr->LPtr;
      //DECLARATION
      SPush (S, ptr->RPtr);      
      ptr = ptr->LPtr;
      //Typ
      load_typ (ptr->key, &typ);
      typs = add_typ_before_types ('F', &typ);
      ptr = STopPop (S);
      //ID
      name = ptr->literal;
      name = add_class_before_name (ActClass, name, &error);
      if (name == NULL){
            parsHT_Dispose(HTable);
            free(typs);
            DStack (S);      
            return (error == 99) ? 99 : 6;       
      }
      ptr = STopPop (S);
      //FUNCTION 2
      ptr = ptr->LPtr;
      //ARG_LIST
      SPush (S, ptr);
      while(SEmpty (S) != 1 && (STop(S))->key == ARG_LIST){ //Load params to typs
            ptr = STopPop (S);
            if (ptr->LPtr != NULL){
                  ptr = push_right_go_left (ptr, S);
                  //DECLARATION
                  ptr = ptr->LPtr;
                  //TYP
                  load_typ (ptr->key, &typ);
                  help = add_char_behind_types (typs, typ);
                  free(typs);
                  if (help == NULL){
                        parsHT_Dispose(HTable);
                        free(name);
                        DStack (S);      
                        return 99;                             
                  }
                  typs = help;           
            }
      }
      if (parsHT_Search(HTable, name, typs) == NULL){
            parsHT_Insert(HTable, name, typs);
            free(name);
            free(typs);           
      }
      else{
            parsHT_Dispose(HTable);
            free(name);
            free(typs);
            DStack (S);
            return 3;      
      }

      *original = ptr;
      return 0;
         
}

int static_var_htinsert(tTNodePtr *original, tStackPtr *S, char *ActClass, parsHT_Table *HTable){
      char typ = '';
      char *typs = NULL;
      char *name = NULL;    
      int error = 0;  
      tTNodePtr ptr; 

      ptr = *original;
      ptr = ptr->LPtr;
      //DECLARATION      
      SPush (S, ptr->RPtr);      
      ptr = ptr->LPtr;
      //Type
      load_typ (ptr->key, &typ); //load typ of var {I,D,S} to typ
      if (typ == 'V'){
            parsHT_Dispose(HTable);
            free(typs)
            DStack (S);
            return 6;                  
      }
      typs = add_typ_before_types ('P', &typ);
      if (typs == NULL){
            parsHT_Dispose(HTable);
            DStack (S);      
            return 99;
      }
      ptr = STopPop (S);
      //ID
      name = ptr->literal;
      name = add_class_before_name (ActClass, name, &error);
      if (name == NULL){
            parsHT_Dispose(HTable);
            free(typs);
            DStack (S);      
            return (error == 99) ? 99 : 6;       
      }
      if (parsHT_Search(HTable, name, typs) == NULL){
            parsHT_Insert(HTable, name, typs);
            free(name);
            free(typs);           
            }
      else{
            parsHT_Dispose(HTable);
            free(name);
            free(typs);
            DStack (S);
            return 3;      
      }

      *original = ptr;
      return 0; 
}


tTNodePtr push_right_go_left (tTNodePtr ptr, tStackPtr *S){
       if (ptr->RPtr != NULL)
            SPush (S, ptr->RPtr);
       return ptr->LPtr;
}

char* add_class_before_name (char *class, char *name, int *error){
      size_t size;
      char* new;

      if (strchr(name, (int)'.') != NULL){
            return NULL;      
      }

      size = strlen(class) + strlen(name) + 2;
      new = malloc (size);
      if (new != NULL){
            new = strcpy(new, class);
            new = strcat(new, ".");
            new = strcat(new, name);
      }
      else
            *error = 99;

      return new;          
}

void load_typ (node_t key, char *typ){
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

char* add_typ_before_types (char typ, char *typs){
      size_t size;
      char* new;
      
      size = strlen(typs) + 2;
      new = malloc (size);
      if (new != NULL){
            new = strcpy(new, &typ);
            new = strcat(new, typs);
      }
      return new;
}

char *add_char_behind_types (char *typs, char c){
      size_t size;
      char* new;
      
      size = strlen(typs) + 2;
      new = malloc (size);
      if (new != NULL){
            new = strcpy(new, typs);
            new = strcat(new, &c);
      }

      return new;
}
