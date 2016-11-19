#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "expr.h"
#include "scanner.h"
#include "precedence_stack.h"
#include "token_buffer.h"
#include "error_codes.h"
#include "memory_constants.h"
#include "parser.h"
#define SIZE 18


extern char* current_class;
extern symbol_table_item_t current_function;
extern tDLList * global_inst_tape;
extern constant_t * mem_constants;


tVar * result_var;

tDLList * work_tape;

char precedence_table[SIZE][SIZE] = {
//input token
      /*  +   -   *   /   (   )   <   >  <=  >=  ==   !=  &&  ||  id  lit  $   ! */ //stack top
        {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // +
        {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // -
        {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // *
        {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // /
        {'<','<','<','<','<','=','<','<','<','<','<','<','<','<','<', '<',' ','<'}, // (
        {'>','>','>','>',' ','>','>','>','>','>','>','>','>','>',' ', ' ','>','>'}, // )
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

int expr_check(PStack *P){
    if(P->top->term != P_EXPR || P->top->LPtr->LPtr->term != P_EXPR){
                return 0;
    }
 return 1;
}

char decode_table(int top_term,int input_token){


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

int choose_rule(PStack *P,token_t *t){

    PStack_item *top_item = PSTopTermPtr(P);
    PStack_item *result_item = malloc(sizeof(PStack_item));
    PStack_item *top;

    switch(top_item->term){

        //pravidlo E -> int_literal,double_literal,...
        case P_LIT:
           printf("Hodnota jeeee: %d\n",top_item->value.i);
           printf("Data type jeeee: %d\n",top_item->value.data_type);
           switch(top_item->value.data_type){

                case INT:
                    result_item->value.data_type = INT;
                    result_item->value.i = top_item->value.i;
                    break;
                case DOUBLE:
                    result_item->value.data_type = DOUBLE;
                    result_item->value.d = top_item->value.d;
                    break;
                case STRING:
                    result_item->value.data_type = STRING;
                    result_item->value.s = top_item->value.s;
                    break;
                case BOOLEAN:
                    result_item->value.data_type = BOOLEAN;
                    result_item->value.b = top_item->value.b;
                    break;

           }



            //printf("Typ tokenu je: %d\n",t->type);
            break;
        // E->id,special_id
        case P_ID:


            break;
        //E-> E + E
        case P_ADD:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

             break;

        //E -> E - E
        case P_SUB:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

            break;
        // E -> E * E
        case P_MUL:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

            break;
        //E -> E / E
        case P_DIV:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

             break;
        //E -> (E)
        case P_RB:
            if(PSTopTermPtr(P)->LPtr->term != P_EXPR || PSTopTermPtr(P)->LPtr->LPtr->term != P_LB){
                printf("Chyba pravidla pre zatvorku");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

            break;
        case P_LESS:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

            break;
        case P_LESSE:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

            break;
        case P_GRT:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }

            break;
        case P_GRE:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
        case P_EQL:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            break;
        case P_NEQL:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            break;
        case P_AND:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            break;

        case P_OR:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            break;
        }
        while(P->top->term != P_HANDLE){
            PSPop(P);
        }
        PSPop(P);
        PSPush(P,P_EXPR);
        //P->top->value.data_type = INT;
        printf("Term: %d\n",P->top->term);

        //top->value.data_type = result_item->value.data_type;
        //top->value = result_item->value;


        P->top->value.data_type = result_item->value.data_type;
        P->top->value = result_item->value;
        printf("term ------%d\n",P->top->value.data_type);
return 1;
}

int init_item(PStack *P,token_t *t){

    PStack_item *push_item = P->top;
    symbol_table_item_t *item;

        switch(t->type){

            case ID:
                //printf("Value of literal is:%s\n",t->string_value);
                //symbol_table_item_t *item;
                if (current_function.id_name != NULL) {

                    symbol_table_t *function = get_symbol_table_for_function(current_class,current_function.id_name);
                    printf("Current function is:%s\n",current_function.id_name);
                    printf("premenna %s \n", t->string_value);
                    item = get_symbol_table_function_item(function,t->string_value);
                    if (item == NULL) {
                        item = get_symbol_table_class_item(current_class, t->string_value);
                    }
                    printf("hodnota %d \n", item->variable.i);
                    printf("data type %d\n", item->variable.data_type);
                    //item->variable.i = 42;
                    switch(item->variable.data_type){
                        case INT:
                             push_item->value.i = 50;
                             push_item->value.data_type = INT;
                            printf("Hodnota premennej: %d\n",item->variable.i);
                            break;
                        case DOUBLE:
                             push_item->value.d = item->variable.d;
                             push_item->value.data_type = DOUBLE;
                            break;
                        case STRING:
                             push_item->value.s = item->variable.s;
                             push_item->value.data_type = STRING;
                            break;
                        case BOOLEAN:
                             push_item->value.b = item->variable.b;
                             push_item->value.data_type = BOOLEAN;
                            break;
                    }

                }
                else {
                    item = get_symbol_table_class_item(current_class, t->string_value);
                    printf("data_type id %d\n", item->variable.data_type);
                }

                result_var = &item->variable;


                //printf("Hodnota premennej: %d\n",item->variable.i);

                //P->top->value.i = 42;


                break;
            case SPECIAL_ID:
                 printf("Value of literal is:%s\n",t->string_value);
                 //TODO pozriet sa do TS  a zistit hodnotu

                item = get_symbol_table_special_id_item(t->string_value);
                 //t->variable.i
                 printf("data_type special id %d\n", item->variable.data_type);

                    switch(item->variable.data_type){
                        case INT:
                            push_item->value.i = item->variable.i;
                            push_item->value.data_type = INT;
                            break;
                        case DOUBLE:
                            push_item->value.d = item->variable.d;
                            push_item->value.data_type = DOUBLE;
                            break;
                        case STRING:
                            push_item->value.s = item->variable.s;
                            push_item->value.data_type = STRING;
                            break;
                        case BOOLEAN:
                            push_item->value.b = item->variable.b;
                            push_item->value.data_type = BOOLEAN;
                            break;
                    }
                 break;


            case INT_LITERAL:
                //printf("Value of literal is:%d\n",t->int_value);
                push_item->value.i = t->int_value;
                push_item->value.data_type = INT;
                tVar *i = insert_int_const(&mem_constants, t->int_value);
                result_var = i;
                break;
            case DOUBLE_LITERAL:
                //printf("Value of literal is:%f\n",t->double_value);
                push_item->value.d = t->double_value;
                push_item->value.data_type = DOUBLE;
                tVar *d = insert_double_const(&mem_constants, t->double_value);
                result_var = d;
                break;
            case STRING_LITERAL:
               // printf("Value of literal is:%s\n",t->string_value);
                push_item->value.s = t->string_value;
                push_item->value.data_type = STRING;
                tVar *s = insert_string_const(&mem_constants, t->string_value);
                result_var = s;
                break;
            case TRUE:
               // printf("Value of literal is:%d\n",true);
                push_item->value.b = true;
                push_item->value.data_type = BOOLEAN;
                tVar *bt = insert_boolean_const(&mem_constants, true);
                result_var = bt;
                break;
            case FALSE:
               // printf("Value of literal is:%d\n",false);
                push_item->value.b = false;
                push_item->value.data_type = BOOLEAN;
                tVar *bf = insert_boolean_const(&mem_constants, false);
                result_var = bf;
                break;
            case ADD:
                printf("Scitanie\n");
                P->top->value.i = 99;
                P->top->value.data_type = 22;
                break;
        }
        //P->top = push_item;

return 1;
}



symbol_table_item_t * expr_res;


int get_psa(token_buffer_t *buffer,symbol_table_item_t * expr_result){
    printf("class %s\n", current_class);
    if (current_function.id_name != NULL) {
        printf("vo funkcii %s\n", current_function.id_name);
        symbol_table_item_t *function = get_symbol_table_class_item(current_class, current_function.id_name);
        work_tape = function->function.instruction_tape;
        //void * x = insert_string_const(&mem_constants, "Hello World Function");
        //DLInsertLast(work_tape, generate(I_PRINT, x, NULL, NULL));
    }
    else {
               work_tape = global_inst_tape;
                /*void * x = insert_string_const(&mem_constants, "Hello World Global");
                DLInsertLast(work_tape, generate(I_PRINT, x, NULL, NULL));*/
    }
      token_t *t = NULL;
      token_t end;
      end.type = ENDMARK;
      add_token_to_buffer(buffer,&end);
      //char result;
      PStack *P = PSInit();
      PSPush(P,P_ENDMARK);
      //printf("term %d\n",P->top->value.data_type);


        buffer_position = 0;
        printf("in PSO %d:\n", buffer->length);
        //token_t *t;
        //t = get_next_token_psa(buffer);
        //printf("TOKEN type: %d\n",t->type);
        t = get_next_token_psa(buffer);
        do{

           // printf("TOKEN type: %d\n",t->type);
            /*if((t = get_next_token_psa(buffer)) == NULL){
                printf("KOncim\n");
                break;
            }*/
             //printf("Pouzit pravidlo: %c\n",decode_table(P,decode_token_array[t->type]));
                int r = PSTopTerm(P);
            switch(decode_table(r,decode_token_array[t->type])){

                case '=':
                    PSPush(P,decode_token_array[t->type]);
                    t = get_next_token_psa(buffer);
                    break;
                case '<':
                    insert_handle(P,PSTopTermPtr(P));
                    PSPush(P,decode_token_array[t->type]);
                    init_item(P,t);
                    switch(P->top->value.data_type){
                        case INT:
                            printf("Value of literal is:%d\n",P->top->value.i);
                            break;
                        case DOUBLE:
                            printf("Value of literal is:%f\n",P->top->value.d);
                            break;
                        case STRING:
                            printf("Value of literal is:%s\n",P->top->value.s);
                            break;
                        case BOOLEAN:
                            printf("Value of literal is:%d\n",P->top->value.b);
                            break;
                    }

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
                    fprintf(stderr,"Unexpected expression.\n");
                    cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
                    break;



            }

            /*printf("top je term %d\n",P->top->term);
            printf("top term na zasobniku: %d\n",PSTopTerm(P));
            printf("TOKEN type: %d\n",t->type);
            printf("Toto je %d\n",decode_token_array[t->type]);
            if(t->type == ENDMARK){

            }*///PSPrint(P);
                /*if(PSTopTerm(P)== P_ENDMARK && t->type == ENDMARK){
                    printf("KOncime uspesne\n");
                    printf("TOP term je : %d\n",PSTopTerm(P));
                    break;
                }*/
           //PSPrint(P);

       }while(PSTopTerm(P) != P_ENDMARK || t->type != ENDMARK);

       expr_result->id_name = "expr_result";
       if (result_var == NULL) {
           printf("faul\n");
       }

       printf("priradujem %p\n", result_var);
       expr_result->variable = *result_var;



       // duri aj takto to ide, asi lepsie
       // while(PSTopTerm(P) != P_ENDMARK || t->type != ENDMARK);
       printf("TOP term je : %d\n",PSTopTerm(P));
       printf("TOP term je : %d\n",PSTopTermPtr(P)->RPtr->term);
       if(PSTopTermPtr(P)->RPtr->RPtr == NULL){
        printf("Uspech\n");
       }
       printf("Na konci:\n");
       PSPrint(P);
       printf("Data type of result is:%d\n",P->top->value.data_type);
       //printf("Result value is:%d\n",P->top->value.i);
        return P->top->value.data_type;


}
