#include "precedence_stack.h"


PStack *PSInit(){
    
     PStack *P = malloc(sizeof(PStack));    
     if(P == NULL){
        return NULL;
     }
     enum Terminals term = P_ENDMARK; // $ kt potrebujeme vlozit na zaciatok zasobnika
     P->top = NULL;
     P->first = NULL;
    
    /*P->top = malloc(sizeof(PStack_item));
    if(P->top == NULL){
        return NULL;
    }
    P->first = malloc(sizeof(PStack_item));
    if(P->first == NULL){
        free(P->top);
        return NULL;
    }*/
    
    PStack_item *new_item = malloc(sizeof(PStack_item));
    if(new_item == NULL){
        //TODO free top a first
        
        return NULL;
    }
    
    new_item->LPtr = NULL;
    new_item->RPtr = NULL;
    new_item->term = term;
    P->top = new_item;
    P->first = new_item;
    return P;
}

void PSPush(PStack *P,enum Terminals term){
    
    PStack_item *term_item;
    term_item = malloc(sizeof(PStack_item));
    if(term_item == NULL){
        return;
    }

    term_item->LPtr = P->top;
    term_item->RPtr = NULL;
    term_item->term = term;
    

    if(P->top != NULL){
        P->top->RPtr = term_item;
    }
    P->top = term_item;
}

/*void PSPop(PStack *P){
    //TODO daco aj vratit ? 
    PStack_item tmp = P->top;
    P->top = tmp->LPtr;
    P->top->RPtr = NULL;
    free(tmp);
}
*/
int PSTopTerm(PStack *P){
    
    PStack_item *top_term = P->top;
    if(top_term->term <= P_ENDMARK){
        return top_term->term;
    }
    else{//TODO prehladavat stack a hladat v nom terminal najblizsie vrcholu
        top_term = top_term->LPtr;
        while(!is_top_terminal(top_term->term)){
            top_term = top_term->LPtr;
            if(top_term == P->first)
                return 42; //TODO ked sa najde az zaciatocny $*/
        }
        return top_term->term;
    }
        return 42;
}    


void PSDestroy(PStack *P){
    //TODO rusenie,este nieco pridat 
    PStack_item *tmp;
    while(P->top != NULL){
        tmp = P->top;
        P->top = tmp->LPtr;
        free(tmp);
    }
}

bool is_top_terminal(int term){
      if(term <= P_ENDMARK){
          return true;
      }
      else
          return false;
}

