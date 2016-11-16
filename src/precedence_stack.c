#include "precedence_stack.h"


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
    if(top_term->term <= P_ENDMARK){
        return top_term->term;
    }/*
    else{//TODO prehladavat stack a hladat v nom terminal najblizsie vrcholu
        top_term = top_term->LPtr;
        while(!is_top_terminal(top_term->term)){
            top_term = top_term->LPtr;
            if(top_term == P->first)
                return 42; //TODO ked sa najde az zaciatocny $
        }
        return top_term->term;
    } */
        //return 42;
}    

PStack_item *PSTopTermPtr(PStack *P){
    
    PStack_item *top_term = P->top;
    if(top_term->term <= P_ENDMARK){
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

void PSDestroy(PStack *P){
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
      if(term <= P_ENDMARK){
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
    while(P->top != NULL){
            //if(P->top->RPtr != NULL){
                printf("Prvok zasobnika je: %d\n",P->top->term);
                P->top = P->top->LPtr;
                
            //}
        }
    return;
        
}   