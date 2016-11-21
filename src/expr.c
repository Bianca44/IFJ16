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
#include "debug.h"
#define SIZE 18


extern char* current_class;
extern symbol_table_item_t current_function;
extern tDLList * global_inst_tape;
extern constant_t * mem_constants;


tVar * top_expr_variable;

tDLList * work_tape;
tVar* op_1;
tVar* op_2;
tVar* tmp;
tVar *var = NULL;
char precedence_table[SIZE][SIZE] = {
//input token
      /*  +   -   *   /   (   )   <   >  <=  >=  ==   !=  &&  ||  id  lit  $   ! */ //stack top
        {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // +
        {'>','>','<','<','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // -
        {'>','>','>','>','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // *
        {'>','>','>','>','<','>','>','>','>','>','>','>','>','>','<', '<','>','<'}, // /
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
    
    int first_operand;
    int second_operand;
    if(result_item == NULL){
        fprintf(stderr,"Unable to allocate memory.\n");
        exit(42); //TODO
    }
    switch(top_item->term){

        //pravidlo E -> int_literal,double_literal,...
        case P_LIT:
           printf("Hodnota jeeee: %d\n",top_item->value.i);
           printf("Data type jeeee: %d\n",top_item->value.data_type);
           switch(top_item->value.data_type){

                case INT:
                   result_item->value.data_type = INT; // TODO odmazat potom
                   //result_item->value.i = top_item->value.i;
                   d_int(top_item->expr->i);
                   result_item->expr = top_item->expr;
                    break;
                case DOUBLE:
                    result_item->value.data_type = DOUBLE;
                     //esult_item->value.d = top_item->value.d;
                    result_item->expr = top_item->expr;
                    break;
                case STRING:
                    result_item->value.data_type = STRING;
                   result_item->expr = top_item->expr;
                    break;
                case BOOLEAN:
                    result_item->value.data_type = BOOLEAN;
                    result_item->expr = top_item->expr;
                    break;

           }



            //printf("Typ tokenu je: %d\n",t->type);
            break;
        // E->id,special_id
        case P_ID:
        printf("Hodnota jeeee: %d\n",top_item->value.i);
        printf("Data type jeeee: %d\n",top_item->value.data_type);
        switch(top_item->value.data_type){

             case INT:
                 result_item->value.data_type = INT;
                 result_item->expr = top_item->expr;
                 d_int(top_item->expr->i);
                 break;
             case DOUBLE:
                 result_item->value.data_type = DOUBLE;
                 result_item->expr = top_item->expr;
                 d_int(top_item->expr->i);
                 break;
             case STRING:
                 result_item->value.data_type = STRING;
                  result_item->expr = top_item->expr;
                 break;
             case BOOLEAN:
                 result_item->value.data_type = BOOLEAN;
                  result_item->expr = top_item->expr;
                 break;

        }


            break;
        //E-> E + E
        case P_ADD:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            
            op_1 = P->top->LPtr->LPtr->expr;
            op_2 = P->top->expr;

             first_operand = P->top->LPtr->LPtr->value.data_type;
             second_operand = P->top->value.data_type;
            
            // int + int
            if(first_operand == INT && second_operand == INT){
                tmp = generate_tmp_var(result_item->value.data_type);
                DLInsertLast(work_tape, generate(I_ADD, op_1, op_2, tmp));

            } //int + double or double + int
            else if((first_operand == INT &&  second_operand == DOUBLE) || (second_operand == INT && first_operand == DOUBLE)){

                result_item->value.data_type = DOUBLE;
                tmp = generate_tmp_var(result_item->value.data_type);
                var =  generate_tmp_var(result_item->value.data_type);
                if(first_operand == INT){
                    i_conv_i_to_d(op_1,NULL,var);
                    DLInsertLast(work_tape, generate(I_ADD, var, op_2, tmp));
                }
                else{
                    i_conv_i_to_d(op_2,NULL,var);
                    DLInsertLast(work_tape, generate(I_ADD, op_1, var, tmp));
                }

            }else if(first_operand == DOUBLE && second_operand == DOUBLE){
                result_item->value.data_type = DOUBLE;
                tmp = generate_tmp_var(result_item->value.data_type);
                DLInsertLast(work_tape, generate(I_ADD, op_1, op_2, tmp));
                
            }
            else if(first_operand == STRING && (second_operand == INT || second_operand == DOUBLE || second_operand == BOOLEAN)){
               // d_int(P->top->expr->i);
                result_item->value.data_type = STRING;
                tmp = generate_tmp_var(result_item->value.data_type);
                var =  generate_tmp_var(result_item->value.data_type);
                i_to_str(op_2,op_1,var);
                DLInsertLast(work_tape, generate(I_CAT, op_1, var, tmp));
                
                
            }
            else if(second_operand == STRING  && (first_operand == INT || first_operand == DOUBLE || first_operand == BOOLEAN)){

                result_item->value.data_type = STRING;
                tmp = generate_tmp_var(result_item->value.data_type);
                var =  generate_tmp_var(result_item->value.data_type);
                i_to_str(op_1,NULL,var);
                DLInsertLast(work_tape, generate(I_CAT, var, op_2, tmp));
            }
            else if(first_operand == STRING && second_operand == STRING){
                result_item->value.data_type = STRING;
                tmp = generate_tmp_var(result_item->value.data_type);
                DLInsertLast(work_tape, generate(I_CAT, op_1, op_2, tmp));
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
            }
            
            result_item->expr = tmp;
             break;

        //E -> E - E
        case P_SUB:
            if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            op_2 = P->top->expr;

            if(first_operand == INT && second_operand == INT){
               
                result_item->value.data_type = INT;
                tmp = generate_tmp_var(result_item->value.data_type);
                DLInsertLast(work_tape, generate(I_SUB, op_1, op_2, tmp));

            }
            else if((first_operand == INT &&  second_operand == DOUBLE) || (second_operand == INT && first_operand == DOUBLE)){
                result_item->value.data_type = DOUBLE;

                tmp = generate_tmp_var(result_item->value.data_type);
                var =  generate_tmp_var(result_item->value.data_type);
                if(first_operand == INT){
                    i_conv_i_to_d(op_1,NULL,var);
                    DLInsertLast(work_tape, generate(I_SUB, var, op_2, tmp));
                }
                else{
                    i_conv_i_to_d(op_2,NULL,var);
                    DLInsertLast(work_tape, generate(I_SUB, op_1, var, tmp));
                }
            }
            else if(first_operand == DOUBLE && second_operand == DOUBLE ){
                result_item->value.data_type = DOUBLE;
                tmp = generate_tmp_var(result_item->value.data_type);
                DLInsertLast(work_tape, generate(I_SUB, op_1, op_2, tmp));

            }
            else if(first_operand == STRING || second_operand == STRING){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

            }else if(first_operand == BOOLEAN || second_operand == BOOLEAN){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
            }
           
            result_item->expr = tmp;

            break;
        // E -> E * E
        case P_MUL:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
             //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if(first_operand == INT && second_operand == INT){
                printf("HOdnota druheho operandu: %d\n",P->top->value.i);
                printf("HOdnota prveho operandu: %d\n",P->top->LPtr->LPtr->value.i);
                result_item->value.data_type = INT;
                //just for test
                result_item->value.i = P->top->value.i * P->top->LPtr->LPtr->value.i;
            }
            else if((first_operand == INT && second_operand == DOUBLE) || (second_operand == INT && first_operand == DOUBLE)){
                result_item->value.data_type = DOUBLE;
            }
            else if(first_operand == STRING || second_operand == STRING){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
            }
            else if(first_operand == BOOLEAN || second_operand == BOOLEAN){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
            }
            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_MUL, op_1, op_2, tmp));
            result_item->expr = tmp;

            break;
        //E -> E / E
        case P_DIV:
              if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
           
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
             //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if(first_operand == INT && second_operand == INT){
                //if(op2->value.i == 0){
                 //   printf("Delenie 0.\n");
                 //   cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
              
                //TODO mozeme delit 2 INT a mat INT vysledok ?
                result_item->value.data_type = INT;
                //result_item->value.i = P->top->LPtr->LPtr->value.i / P->top->value.i;
            }
            else if(first_operand == DOUBLE && second_operand == INT){
                //if(op2->value.i == 0){
                //    printf("Delenie 0.\n");
                 //   cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                //}
                result_item->value.data_type = DOUBLE;
            }
            else if (first_operand == INT && second_operand == DOUBLE){
               // if(op2->value.d == 0.0){
                //    printf("Delenie 0.\n");
                //    cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
               // }
                result_item->value.data_type = DOUBLE;
            }
            else if(first_operand == STRING || second_operand == STRING){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
            }
            else if(first_operand == BOOLEAN || second_operand == BOOLEAN){
                printf("Incompatible data types.\n");
                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
            }

            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_DIV, op_1, op_2, tmp));
            result_item->expr = tmp;

             break;
        //E -> (E)
        case P_RB:
             if(PSTopTermPtr(P)->LPtr->term != P_EXPR || PSTopTermPtr(P)->LPtr->LPtr->term != P_LB){
                printf("Chyba pravidla pre zatvorku");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            switch(P->top->LPtr->value.data_type){
                case INT:
                    result_item->value.data_type = INT;
                    //result_item->value.i = P->top->LPtr->value.i;
                    break;
                case DOUBLE:
                    result_item->value.data_type = DOUBLE;
                    //result_item->value.d = P->top->LPtr->value.d;
                    break;
                case STRING:
                    result_item->value.data_type = STRING;
                    //result_item->value.s = P->top->LPtr->value.s;
                    break;
                case BOOLEAN:
                    result_item->value.data_type = BOOLEAN;
                    //result_item->value.b = P->top->LPtr->value.b;
                    break;
            }
            result_item->expr = P->top->LPtr->expr;
            
            break;
        case P_LESS:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
             //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            //TODO boolean < int/double resp int/double < boolean
            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_L, op_1, op_2, tmp));
            result_item->expr = tmp;
            break;

        case P_LESSE:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
             //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            //TODO boolean < int/double resp int/double < boolean
             tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_LE, op_1, op_2, tmp));
            result_item->expr = tmp;
            break;

        case P_GRT:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
             op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
            //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            //TODO boolean < int/double resp int/double < boolean
            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_G, op_1, op_2, tmp));
            result_item->expr = tmp;

            break;
        case P_GRE:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
            //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_GE, op_1, op_2, tmp));
            result_item->expr = tmp;

            //TODO boolean < int/double resp int/double < boolean

        case P_EQL:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
            //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_E, op_1, op_2, tmp));
            result_item->expr = tmp;
            //TODO boolean < int/double resp int/double < boolean

            break;
        case P_NEQL:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
            //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            tmp = generate_tmp_var(result_item->value.data_type);
            DLInsertLast(work_tape, generate(I_NE, op_1, op_2, tmp));
            result_item->expr = tmp;
            //TODO boolean < int/double resp int/double < boolean

            break;
        case P_AND:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
            //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            //TODO boolean < int/double resp int/double < boolean
            //TODO este neni instrukcia pre AND
            //tmp = generate_tmp_var(result_item->value.data_type);
            //DLInsertLast(work_tape, generate(I_G, op_1, op_2, tmp));
           // result_item->expr = tmp;
            break;

        case P_OR:
             if(!expr_check(P)){
                printf("Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            first_operand = P->top->LPtr->LPtr->value.data_type;
            second_operand = P->top->value.data_type;
            op_1 = P->top->LPtr->LPtr->expr;
            //d_int(P->top->LPtr->LPtr->expr->i);
            //d_int(P->top->expr->i);
            op_2 = P->top->expr;
            if((first_operand == INT && second_operand == INT) || (first_operand == INT && second_operand == DOUBLE) || (first_operand == DOUBLE && second_operand == INT)){
                result_item->value.data_type = BOOLEAN;
            }
            else if(first_operand == STRING || second_operand == STRING){
                fprintf(stderr,"Chybny vyraz.\n");
                cleanup_exit(SYNTACTIC_ANALYSIS_ERROR);
            }
            else if(first_operand == BOOLEAN && second_operand == BOOLEAN ){
                result_item->value.data_type = BOOLEAN;
            }
            //TODO boolean < int/double resp int/double < boolean

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
        P->top->expr = result_item->expr;
        
return 1;
}


int init_item(PStack *P,token_t *t){

    PStack_item *push_item = P->top;
    symbol_table_item_t *item;

        switch(t->type){

            case ID:
               
                if (current_function.id_name != NULL) {

                    symbol_table_t *function = get_symbol_table_for_function(current_class,current_function.id_name);
                    printf("Current function is:%s\n",current_function.id_name);
                    printf("premenna %s \n", t->string_value);
                    item = get_symbol_table_function_item(function,t->string_value);
                    if(item == NULL){
                    item = get_symbol_table_class_item(current_class, t->string_value);
                    }
                  
                    switch(item->variable.data_type){
                        case INT:
                             
                             push_item->value.data_type = INT;
                             push_item->expr = &item->variable;
                              d_int(push_item->expr->i);
                            break;
                        case DOUBLE:
                            
                             push_item->value.data_type = DOUBLE;
                             push_item->expr = &item->variable;
                            break;
                        case STRING:
                        
                             push_item->value.data_type = STRING;
                             push_item->expr = &item->variable;
                            break;
                        case BOOLEAN:
                     
                             push_item->value.data_type = BOOLEAN;
                             push_item->expr = &item->variable;
                            break;
                    }

                }
                else {
                    item = get_symbol_table_class_item(current_class, t->string_value);
                    switch(item->variable.data_type){
                        case INT:
                            
                             push_item->value.data_type = INT;
                             push_item->expr = &item->variable;
                             d_int(push_item->expr->i);
                                                    
                             break;
                        case DOUBLE:
                             
                             push_item->value.data_type = DOUBLE;
                             push_item->expr = &item->variable;
                            break;
                        case STRING:
                             
                             push_item->value.data_type = STRING;
                             push_item->expr = &item->variable;
                            break;
                        case BOOLEAN:
                             
                             push_item->value.data_type = BOOLEAN;
                             push_item->expr = &item->variable;
                            break;
                    }
                }


                break;
            case SPECIAL_ID:
                 printf("Value of literal is:%s\n",t->string_value);
                 
                item = get_symbol_table_special_id_item(t->string_value);
                
                 printf("data_type special id %d\n", item->variable.data_type);

                    switch(item->variable.data_type){
                        case INT:
                           
                            push_item->value.data_type = INT;
                            push_item->expr = &item->variable;
                            break;
                        case DOUBLE:
                            
                            push_item->value.data_type = DOUBLE;
                            push_item->expr = &item->variable;
                            break;
                        case STRING:
                           
                            push_item->value.data_type = STRING;
                            push_item->expr = &item->variable;
                            break;
                        case BOOLEAN:
                            
                            push_item->value.data_type = BOOLEAN;
                            push_item->expr = &item->variable;
                            break;
                    }

                 
                 break;


            case INT_LITERAL:
                //printf("Value of literal is:%d\n",t->int_value);
                push_item->value.i = t->int_value;
                push_item->value.data_type = INT;
                tVar *i = insert_int_const(&mem_constants, t->int_value);
                push_item->expr = i;
                break;
            case DOUBLE_LITERAL:
                //printf("Value of literal is:%f\n",t->double_value);
                push_item->value.d = t->double_value;
                push_item->value.data_type = DOUBLE;
                tVar *d = insert_double_const(&mem_constants, t->double_value);
                push_item->expr = d;
                break;
            case STRING_LITERAL:
               // printf("Value of literal is:%s\n",t->string_value);
                push_item->value.s = t->string_value;
                push_item->value.data_type = STRING;
                tVar *s = insert_string_const(&mem_constants, t->string_value);
                push_item->expr = s;
                break;
            case TRUE:
               // printf("Value of literal is:%d\n",true);
                push_item->value.b = true;
                push_item->value.data_type = BOOLEAN;
                tVar *bt = insert_boolean_const(&mem_constants, true);
                push_item->expr = bt;
                break;
            case FALSE:
               // printf("Value of literal is:%d\n",false);
                push_item->value.b = false;
                push_item->value.data_type = BOOLEAN;
                tVar *bf = insert_boolean_const(&mem_constants, false);
                push_item->expr = bf;
                break;
            
        }

        

return 1;
}



symbol_table_item_t * expr_res;

tVar * generate_tmp_var(int data_type) {
    if (current_function.id_name != NULL) {
        top_expr_variable = &insert_tmp_variable_symbol_table_function(current_function.id_name, data_type)->variable;
    } else {
        top_expr_variable = &insert_tmp_variable_symbol_table_class(data_type)->variable;
    }

    return top_expr_variable;
}


int get_psa(token_buffer_t *buffer,symbol_table_item_t * st_item, tVar** expr_result) {
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

            
       }while(PSTopTerm(P) != P_ENDMARK || t->type != ENDMARK);

       st_item->id_name = "expr_result";
       st_item->variable = *P->top->expr;
       *expr_result = P->top->expr;


       


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
