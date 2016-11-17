#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "expr.h"
#include "scanner.h"
#include "precedence_stack.h"
#include "token_buffer.h"
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
[EQUAL] = P_EQL,
[ENDMARK] = P_ENDMARK
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

void choose_rule(PStack *P,token_t *t){

    PStack_item *top_item = PSTopTermPtr(P);
    //PStack_item *expr_item = malloc(sizeof(PStack_item));
    switch(top_item->term){

        //pravidlo E -> int_literal,double_literal,...
        case P_LIT:
            //odstranim <lit
            //printf("TOP je: %d\n",P->top->term);
            PSPop(P);
            PSPop(P);

            //TODO neprepisat si s t data_type !!!!!
            PSPush(P,P_EXPR);

            break;  

         
                

    }

}





int get_psa(token_buffer_t *buffer){
      token_t *t = NULL;
      token_t end;
      end.type = ENDMARK;
      add_token_to_buffer(buffer,&end);
       
      PStack *P = PSInit();
      PSPush(P,P_ENDMARK);
     // printf("term %d\n",P->top->term);
      

        buffer_position = 0;
        printf("in PSO %d:\n", buffer->length);
        //token_t *t;
        t = get_next_token_psa(buffer);
        //printf("TOKEN type: %d\n",t->type);
        
        do{
           // printf("TOKEN type: %d\n",t->type);
            /*if((t = get_next_token_psa(buffer)) == NULL){
                printf("KOncim\n");
                break;
            }*/
             //printf("Pouzit pravidlo: %c\n",decode_table(P,decode_token_array[t->type]));
            switch(decode_table(P,decode_token_array[t->type])){

                case '=':
                    PSPush(P,decode_token_array[t->type]);
                    t = get_next_token_psa(buffer);
                    break;
                case '<':
                    insert_handle(P,PSTopTermPtr(P));
                    PSPush(P,decode_token_array[t->type]);
                    printf("term %d\n",P->top->term);
                   // PSPrint(P); 
                    //printf("TOKEN type: %d\n",t->type);
                     t = get_next_token_psa(buffer);
                    //printf("TOKEN type: %d\n",t->type);
                    break;
                case '>':
                    //PSPrint(P); 
                    choose_rule(P,t);
                    //PSPush(P,P_ADD);
                    //PSPrint(P); 
                    //t = get_next_token_psa(buffer);
                    break;
                case ' ':
                    printf("Chyba\n");
                    return -1;
                    break;



            }
            
            printf("top je term %d\n",P->top->term);
            printf("top term na zasobniku: %d\n",PSTopTerm(P));
            printf("TOKEN type: %d\n",t->type);
            printf("Toto je %d\n",decode_token_array[t->type]);
            if(t->type == ENDMARK){

            }

       }while( t->type != ENDMARK);
       PSPrint(P); 
        return 1;

         
}