#include <stdio.h>
#include <stdlib.h>
#include "expr.h"
#include "precedence_stack.h"
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

char decode_table(PStack *P){

    int top_term = PSTopTerm(P);
    int input_t = P_ADD; // TODO

    return precedence_table[top_term][input_t];

   }

int buffer_position = 0;
token_t* get_next_token_psa(token_buffer_t *token_buf) {
        if (buffer_position < token_buf->length) {
                return token_buf->list[buffer_position++];
        } else {
                return NULL;
        }
}

int get_psa(token_buffer_t *buffer){

      PStack *P = PSInit();
      printf("term %d\n",P->top->term);
      PSPush(P,P_ADD);
      printf("term %d\n",P->top->term);
      PSPush(P,P_SUB);
      printf("term %d\n",P->top->term);
      PSPush(P,P_MUL);
      printf("term %d\n",P->top->term);
      PSPush(P,P_GRT);
      printf("term %d\n",P->top->term);
      PSPush(P,P_EXPR);
      printf("term %d\n",P->top->term);
      PSPush(P,P_EXPR);
      printf("term %d\n",P->top->term);
      PSPush(P,P_ENDMARK);
      printf("term %d\n",P->top->term);
      printf("top term = %d\n",PSTopTerm(P));
      printf("Pravidlo pre %c\n",decode_table(P));
      PSDestroy(P);

        buffer_position = 0;
        printf("in PSO %d:\n", buffer->length);
        token_t *t;
        t = get_next_token_psa(buffer);
        while (t!=NULL) {
                printf("token c. %d\n", t->type);
                t = get_next_token_psa(buffer);
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
