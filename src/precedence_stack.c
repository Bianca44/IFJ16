#include "precedence_stack.h"


int PSInit(PStack *P){
    
     PStack *P = malloc(sizeof(PStack));    
     if(P == NULL){
        return INIT_ERROR;
     }
     Terminals term = P_ENDMARK; // $ kt potrebujeme vlozit na zaciatok zasobnika
     P->top = NULL;
     P->first = NULL;
     
    P->top = malloc(sizeof(PStack_item));
    if(P->top == NULL){
        return INIT_ERROR;
    }
    P->first = malloc(sizeof(PStack_item));
    if(P->first == NULL){
        free(P->top);
        return INIT_ERROR;
    }
    
    PStack_item new_item = malloc(sizeof(PStack_item));
    if(new_item == NULL){
        //TODO free top a first
        free(P->top);
        free(P->first);
        return INIT_ERROR;
    }
    
    new_item->LPtr = NULL;
    new_item->RPtr = NULL;
    new_item->term = term;
    new_item->t_type = NULL; //TODO co tu dat
    P->top = new_item;
    P->first = new_item;

}

void PSPush(PStack *P,Terminal term){
    
    PStack_item term_item;
    term_item = malloc(sizeof(PStack_item));
    if(term_item == NULL){
        return;
    }
    term_item->LPtr = P->top;
    term_item->RPtr = NULL;
    term_item->term = term;
    term_item->t_type = NULL;   //TODO

    if(P->top != NULL){
        P->top->RPtr = term_item;
    }
    P->top = term_item;
}

void PSPop(PStack *P){
    //TODO daco aj vratit ? 
    PStack_item tmp = P->top;
    P->top = tmp->LPtr;
    P->top->RPtr = NULL;
    free(tmp);
}

int PSTopTerm(PStack *P){
    
    PStack_item top_term = P->top;
    if(top_term->term <= P_ENDMARK){
        return top_term->term;
    }
    else{//TODO prehladavat stack a hladat v nom terminal najblizsie vrcholu
        top_term = top_term->LPtr;
        while(!is_top_terminal(top_term->term)){
            top_term = top_term->LPtr;
            if(top_term == P->first)
                return INIT_ERROR; //TODO ked sa najde az zaciatocny $
        }
        return top_term->term;
    }
}    


void PSDestroy(PStack *P){
    //TODO rusenie,este nieco pridat 
    PStack_item tmp;
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

