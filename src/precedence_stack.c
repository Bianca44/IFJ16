#include "precedence_stack.h"
#include "token_buffer.h"
#include "debug.h"


char *prec_names[20] = { "P_ADD","P_SUB","P_MUL","P_DIV","P_LB", "P_RB",     // )
                                "P_LESS","P_GRT","P_LESSE", "P_GRE","P_EQL", "P_NEQL",   // !=
                                "P_AND","P_OR","P_ID","P_LIT","P_ENDMARK", "P_NOT", "P_EXPR", "P_HANDLE"};


PStack *PSInit(){
    
     PStack *P = malloc(sizeof(PStack));    
     if(P == NULL){
        return NULL;
     }
     P->top = NULL;
     P->first = NULL;
 
    return P;
}

void PSPush(PStack *P,enum Terminals term){
    
    PStack_item *term_item;
    term_item = malloc(sizeof(PStack_item));
    if(term_item == NULL){
        return;
    }
    term_item->is_constant = false;
    term_item->expr = NULL;
    term_item->term = term;
    term_item->LPtr = P->top;
    term_item->RPtr = NULL;

    if(P->top != NULL){
        P->top->RPtr = term_item;
    }
    if(P->first == NULL){
        P->first = term_item;
    } 
    P->top = term_item;
 }   

void PSPop(PStack *P){
    
    if(P->top != NULL){
        
        PStack_item *tmp = P->top;
        P->top = tmp->LPtr;
        P->top->RPtr = NULL;
        free(tmp);
    }
}
int PSTopTerm(PStack *P){
    
    PStack_item *top_term = P->top;
    //TODO ide sa skusit NOT, bol tu P_ENDMARK
    if(top_term->term <= P_NOT){
        return top_term->term;
    }
    else{//TODO prehladavat stack a hladat v nom terminal najblizsie vrcholu
        top_term = top_term->LPtr;
        if(top_term->term <= P_NOT){
            return top_term->term;
        }
        while(!is_top_terminal(top_term->term)){
            if(top_term->LPtr != NULL){
                 top_term = top_term->LPtr;
            }
            //if(top_term == P->first)
              //  return 42; //TODO ked sa najde az zaciatocny $
        }
        return top_term->term;
    } 
        return 42;
}    

PStack_item *PSTopTermPtr(PStack *P){
    
    PStack_item *top_term = P->top;
    if(top_term->term <= P_NOT){
        return top_term;
    }
    else{//TODO prehladavat stack a hladat v nom terminal najblizsie vrcholu
        top_term = top_term->LPtr;
        while(!is_top_terminal(top_term->term)){
            top_term = top_term->LPtr;
            if(top_term == P->first)
                return NULL; //TODO ked sa najde az zaciatocny $
        }
        return top_term;
    }
        return NULL;
}    
void insert_handle(PStack *P,PStack_item *item){

   PStack_item *tmp;
   tmp = item->RPtr;
   PStack_item *handle_item;
   handle_item = malloc(sizeof(PStack_item));
   if(handle_item == NULL){
        return;
   }
   handle_item->term = P_HANDLE;
   handle_item->RPtr = item->RPtr;
   handle_item->LPtr = item;
   if(item->RPtr == NULL){
        item->RPtr = handle_item;
        P->top = handle_item;
   }
   else{   
     tmp->LPtr = handle_item;
     item->RPtr = handle_item;
   }

}

void PSDispose(PStack *P){
    //TODO rusenie,este nieco pridat 
    PStack_item *tmp;
    while(P->top != NULL){
        tmp = P->top;
        P->top = tmp->LPtr;
        free(tmp);
    }
    free(P);
}

bool is_top_terminal(int term){
      if(term <= P_NOT){
          return true;
      }
      else
          return false;
}

void PSPrint(PStack *P){

    if(P->first == NULL){
        printf("Je null");
    }
    //TODO pomocna premenna na cyklenie, nech si nedoserem top
    //P->top = P->first;
    PStack_item *tmp = P->top;
    while(tmp != NULL){
            //if(P->top->RPtr != NULL){
               //printf("Prvok zasobnika je: %d\n",tmp->term);
               d_print("Prvok zasobnika je: %s\n",prec_names[tmp->term]);
                tmp = tmp->LPtr;
                
            //}
        }
    return;
        
}   
