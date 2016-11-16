#include <stdio.h>
#include <stdlib.h>
#include "expr.h"
#include "scanner.h"
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

int decode_token_array[ENUM_SIZE] = {
[ADD] = P_ADD,
[SUB] = P_SUB,
[MUL] = P_MUL,
[DIV] = P_DIV,
[LEFT_ROUNDED_BRACKET] = P_LB,
[RIGHT_ROUNDED_BRACKET] = P_RB,
[LOGICAL_AND] = P_AND,
[LOGICAL_OR] = P_OR,
[NEG] = P_NOT,
[INT_LITERAL] = P_LIT,
[STRING_LITERAL] = P_LIT,
[DOUBLE_LITERAL] = P_LIT,
[TRUE] = P_LIT,
[FALSE] = P_LIT,
[NOT_EQUAL] = P_NEQL,
[LESS] = P_LESS,
[LESS_EQUAL] = P_LESSE,
[GREATER] = P_GRT,
[GREATER_EQUAL] = P_GRE,
[ID] = P_ID,
[SPECIAL_ID] = P_ID,
[EQUAL] = P_EQL
};

char decode_table(PStack *P,int input_token){

    int top_term = PSTopTerm(P); 
    return precedence_table[top_term][input_token];

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
      //printf("term %d\n",P->top->term);

      //tVar var;

      PSPush(P,P_ADD);
      printf("term %d\n",P->top->term);
      PSPush(P,P_ENDMARK);
      printf("term %d\n",P->top->term);
      PSPush(P,P_MUL);
      printf("term %d\n",P->top->term);
      PSPush(P,P_GRT);
      printf("term %d\n",P->top->term);
      //PSPush(P,P_EXPR);
      printf("term %d\n",P->top->term);
     printf("Top term je: %d\n",PSTopTerm(P));
      //PSPush(P,P_EXPR);
      printf("term %d\n",P->top->term);
      //PSPop(P);
      //PSPop(P);
      printf("term %d\n",P->top->term);
      //PStack_item *item = PSTopTermPtr(P);
      //printf("term %d\n", (PSTopTermPtr(P))->term);
      //insert_handle(P,PSTopTermPtr(P));
      //PSPrint(P);
      //printf("top term = %d\n",(PSTopTerm(P)));
      //printf("term %d\n",item->RPtr);
      //PStack_item *item2;
      //item2 = item->RPtr;
      //printf("term %d\n",item2->term);
      //PSDestroy(P);

        buffer_position = 0;
        printf("in PSO %d:\n", buffer->length);
        //printf("Hodnota pola je  %d\n",decode_token_array[GREATER] );
        token_t *t;
        t = get_next_token_psa(buffer);
       /* if(t->type == INT_LITERAL){
          var.i = t->int_value;
          printf("hodnota je %d\n",var.i );
        }*/
        //printf("Pravidlo pre %c\n",decode_table(P,ADD));
      
        /*var.data_type = t->type;
        printf("%d\n", var.data_type);
        printf("Hodnota pola je  %d\n",decode_token_array[t->type]);*/
        printf("Top term je: %d\n",PSTopTerm(P));
        printf("Pravidlo je : %c\n", precedence_table[PSTopTerm(P)][decode_token_array[t->type]]);
       /* do {
                //printf("token c. %d\n", t->type);
                t = get_next_token_psa(buffer);
                //var.data_type = t->type;
                //printf("%d\n", var.data_type);
        }while(t!=NULL);
        //printf("cislo tokenu %d\n", t->type);
        return 1;
        PStackInit();
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
