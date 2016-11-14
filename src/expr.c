#include <stdio.h>
#include <stdlib.h>
#include "expr.h"
#define SIZE 18

char precedence_table[SIZE][SIZE] = {
//input token
/*  +   -   *   /   (   )   <   >  <=  >=  ==   !=  &&  ||  id  lit  $   ! */ //stack top
  {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // +
  {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // -
  {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // *
  {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // /
  {'<','<','<','<','<','=','<','<','<','<','<','<','<','<','<', '<',' ','<'}, // (
  {'>','>','>','>',' ','>','>','>','>','>','>','>','>','>',' ', ' ',' ','>'}, // )
  {'<','<','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // <
  {'<','<','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // >
  {'<','<','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // <=
  {'<','<','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // >=
  {'<','<','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // ==
  {'<','<','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // !=
  {'<','<','<','<','<','>','<','<','<','<','<','<','>','>','<', '<','>','<'}, // &&
  {'<','<','<','<','<','>','<','<','<','<','<','<','>','>','<', '<','>','<'}, // ||
  {'>','>','>','>',' ','>','>','>','>','>','>','>','>','>',' ', ' ','>',' '}, // id
  {'>','>','>','>',' ','>','>','>','>','>','>','>','>','>',' ', ' ','>',' '}, // lit
  {'<','<','<','<','<',' ','<','<','<','<','<','<','<','<','<', '<',' ','<'}, // $
  {'>','>','>','>','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // !
};

/*char decode_table(PStack *P){
    
    int top_term = PSTopTerm(P);
    int input_t = get_next_token(); // TODO 
   
    return precedence_table[top_term][input_t]; 
            
}*/

int get_psa(token_buffer_t *buffer){
  printf("in pso\n");
  token_t *t;
  t = get_next_token_buffer(buffer);
  if (t==NULL) {
    printf("je null\n");
  }
  //printf("cislo tokenu %d\n", t->type);
  return 1;
    /*PStackInit();
    int input; 
    do{
        
        switch(decode_table(P)){

            case '=':
    
                break;
            
            case '<':
                break;
            
            case '>':
                break;


    while((PSTopTerm(P)!=P_ENDMARK) && (input!=P_ENDMARK));

  */




            
        

}

