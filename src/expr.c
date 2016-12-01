/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Juraj Ondrej Dúbrava, xdubra03
 */

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
#include <string.h>


/* Switch whether to use INC/DEC instructions */
#define INC_DEC_ON 0
#define SIZE 18


extern char *current_class;
extern symbol_table_item_t current_function;
extern tList *global_inst_tape;
extern constant_t *mem_constants;
extern char * function_name_call;

bool expr_in_function;


char * expr_var_name;

int expr_len;


token_buffer_t *expr_token_buffer;


tVar *top_expr_variable;

tList *work_tape;
tVar *op_1;
tVar *op_2;
tVar *tmp;
tVar *var = NULL;
char precedence_table[SIZE][SIZE] = {

/*token   +   -   *   /   (   )    <    >    <=  >=    ==   !=  &&    ||  id    lit  $   !   stack top   */
        {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // +
        {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // -
        {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // *
        {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // /
        {'<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<'}, // (
        {'>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>'}, // )
        {'<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // <
        {'<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // >
        {'<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // <=
        {'<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // >=
        {'<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // ==
        {'<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // !=
        {'<', '<', '<', '<', '<', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<'}, // &&
        {'<', '<', '<', '<', '<', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<'}, // ||
        {'>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', ' '}, // id
        {'>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', ' '}, // lit
        {'<', '<', '<', '<', '<', ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<'}, // $
        {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<'}, // !
};



/*decoding input token names to precedence analysis tokens */
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
/*control for nonterminals*/
int expr_check(PStack * P) {
        if (P->top->term != P_EXPR || P->top->LPtr->LPtr->term != P_EXPR) {
                return 0;
        }
        return 1;
}
/*cleanup function when error occurs*/
void expr_exit(int exit_code) {
        free_token_buffer_local(expr_token_buffer);
        //PSDispose(P);
        exit(exit_code);
}

/*decoding symbol in precedence table*/
char decode_table(int top_term, int input_token) {


        return precedence_table[top_term][input_token];

}
/* get next token from token buffer for precedence analysis*/
int expr_buffer_position = 0;
token_t *get_next_token_psa(token_buffer_t * token_buf) {
        if (expr_buffer_position < token_buf->length) {
                return token_buf->list[expr_buffer_position++]; /* CHECK TYPE !!!! */
        } else {
                return NULL;
        }
}

/* evaluation of all individual rules in precedence analysis rules*/
/* 3-address code instructions are inserted on the current instruction tape*/
int choose_rule(PStack * P) {

        PStack_item *top_item = PSTopTermPtr(P);
        PStack_item result_item;
        result_item.expr = NULL;
        result_item.value.data_type = 0;
        int first_operand;
        int second_operand;
        d_int(top_item->term);
        switch (top_item->term) {

        /* E rule -> int_literal,double_literal,... */
        case P_LIT:


                d_print("Data type je: %d\n", top_item->value.data_type);
                switch (top_item->value.data_type) {

                case INT:
                        result_item.value.data_type = INT;
                        d_int(top_item->expr->i);
                        result_item.expr = top_item->expr;

                        break;
                case DOUBLE:
                        result_item.value.data_type = DOUBLE;
                        d_dob(top_item->expr->d);
                        result_item.expr = top_item->expr;

                        break;
                case STRING:
                        result_item.value.data_type = STRING;
                        d_str(top_item->expr->s);
                        result_item.expr = top_item->expr;

                        break;
                case BOOLEAN:
                        result_item.value.data_type = BOOLEAN;
                        d_bol(top_item->expr->b);
                        result_item.expr = top_item->expr;

                        break;

                }

                break;

        /* E->id,special_id */
        case P_ID:

                d_print("Data type je: %d\n", top_item->value.data_type);
                switch (top_item->value.data_type) {

                case INT:
                        result_item.value.data_type = INT;
                        result_item.expr = top_item->expr;
                        d_int(top_item->expr->i);
                        break;
                case DOUBLE:
                        result_item.value.data_type = DOUBLE;
                        result_item.expr = top_item->expr;
                        d_dob(top_item->expr->d);
                        break;
                case STRING:
                        result_item.value.data_type = STRING;
                        result_item.expr = top_item->expr;
                        d_str(top_item->expr->s);
                        break;
                case BOOLEAN:
                        result_item.value.data_type = BOOLEAN;
                        result_item.expr = top_item->expr;
                        d_bol(top_item->expr->b);
                        break;

                }


                break;
        /* E-> E + E */
        case P_ADD:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }

                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;

                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);


                /* int + int */
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);

                        //check expression for using inc instruction
                        bool is_inc = INC_DEC_ON && expr_in_function && expr_len == 3 && (op_1->i == 1 || op_2->i == 1) && strcmp(function_variable.id_name, expr_var_name) == 0;

                        if (is_inc) {
                                /* i = i + 1; */
                                tmp = op_1;
                                InsertLast(work_tape, generate(I_INC, op_1, NULL, NULL));
                        } else {
                                InsertLast(work_tape, generate(I_ADD, op_1, op_2, tmp));
                        }

                } /* int + double or double + int */
                else if ((first_operand == INT && second_operand == DOUBLE)
                         || (second_operand == INT && first_operand == DOUBLE)) {

                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_ADD, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_ADD, op_1, var, tmp));
                        }
                /*double + double */
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_ADD, op_1, op_2, tmp));
                /*string concatenation*/
                } else if (first_operand == STRING
                           && (second_operand == INT || second_operand == DOUBLE || second_operand == BOOLEAN)) {
                        // d_int(P->top->expr->i);
                        /*int,double or boolean operand conversion to string*/
                        result_item.value.data_type = STRING;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_TO_STRING, op_2, NULL, var));
                        InsertLast(work_tape, generate(I_CAT, op_1, var, tmp));

                /*string concatenation*/
                } else if (second_operand == STRING
                           && (first_operand == INT || first_operand == DOUBLE || first_operand == BOOLEAN)) {
                        /*int,double or boolean operand conversion to string*/
                        result_item.value.data_type = STRING;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_TO_STRING, op_1, NULL, var));
                        InsertLast(work_tape, generate(I_CAT, var, op_2, tmp));
                /*string + string concatenation*/
                } else if (first_operand == STRING && second_operand == STRING) {
                        result_item.value.data_type = STRING;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_CAT, op_1, op_2, tmp));
                /*boolean + boolean - error*/
                } else if (first_operand == BOOLEAN || second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;

        /* E -> E - E */
        case P_SUB:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);
                /*int - int*/
                if (first_operand == INT && second_operand == INT) {

                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        //check expression for using dec instruction
                        bool is_dec = INC_DEC_ON && expr_in_function && expr_len == 3 && (op_1->i == 1 || op_2->i == 1) && strcmp(function_variable.id_name, expr_var_name) == 0;

                        if (is_dec) {
                                /* i = i - 1; */
                                tmp = op_1;
                                InsertLast(work_tape, generate(I_DEC, op_1, NULL, NULL));
                        } else {
                                InsertLast(work_tape, generate(I_SUB, op_1, op_2, tmp));
                        }
                }/*int - double or double - int*/
                else if ((first_operand == INT && second_operand == DOUBLE)
                         || (second_operand == INT && first_operand == DOUBLE)) {
                        result_item.value.data_type = DOUBLE;

                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_SUB, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_SUB, op_1, var, tmp));
                        }
                /* double - double */
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_SUB, op_1, op_2, tmp));

                } else if (first_operand == STRING || second_operand == STRING) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean - boolean - error*/
                } else if (first_operand == BOOLEAN || second_operand == BOOLEAN) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /* E -> E * E */
        case    P_MUL:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);
                /* int * int */
                if (first_operand == INT && second_operand == INT) {

                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_MUL, op_1, op_2, tmp));


                /* int * double or double * int */
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (second_operand == INT && first_operand == DOUBLE)) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_MUL, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_MUL, op_1, var, tmp));
                        }
                /* double * double */
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_MUL, op_1, op_2, tmp));
                /*string * string - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean * boolean - error*/
                } else if (first_operand == BOOLEAN || second_operand == BOOLEAN) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /* E -> E / E */
        case    P_DIV:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;

                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);
                /* int / int */
                if (first_operand == INT && second_operand == INT) {


                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_DIV, op_1, op_2, tmp));
                /* int / double or double / int */
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (second_operand == INT && first_operand == DOUBLE)) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_DIV, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_DIV, op_1, var, tmp));
                        }
                /* double / double */
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_DIV, op_1, op_2, tmp));
                /*string / string - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean / boolean*/
                } else if (first_operand == BOOLEAN || second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }


                result_item.expr = tmp;

                break;
        /* E -> (E) */
        case P_RB:
                if (PSTopTermPtr(P)->LPtr->term != P_EXPR || PSTopTermPtr(P)->LPtr->LPtr->term != P_LB) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                switch (P->top->LPtr->value.data_type) {
                case INT:
                        result_item.value.data_type = INT;
                        break;
                case DOUBLE:
                        result_item.value.data_type = DOUBLE;
                        break;
                case STRING:
                        result_item.value.data_type = STRING;
                        break;
                case BOOLEAN:
                        result_item.value.data_type = BOOLEAN;
                        break;
                }
                result_item.expr = P->top->LPtr->expr;

                break;
        /* E -> E < E*/
        case P_LESS:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);
                /*int < int */
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_L, op_1, op_2, tmp));
                /*int < double or double < int*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_L, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_L, op_1, var, tmp));
                        }

                /* double < double*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_L, op_1, op_2, tmp));
                /*comparing strings - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                /*boolean < boolean - error*/
                } else if ((first_operand == BOOLEAN && second_operand == BOOLEAN)
                           || (second_operand == BOOLEAN && first_operand == BOOLEAN)) {
                        result_item.value.data_type = BOOLEAN;
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

                /*boolean < int or double - error*/
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*int or double  < boolean - error*/
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;
        /*E -> E <= E*/
        case P_LESSE:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);
                /*int <= int*/
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_LE, op_1, op_2, tmp));

                /*int <= double or double <= int*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_LE, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_LE, op_1, var, tmp));
                        }

                /*double <= double*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_LE, op_1, op_2, tmp));
                /*comparing strings - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean <= boolean - error*/
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean <= int or double */
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*int or double <= boolean*/
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;
        /*E -> E > E */
        case P_GRT:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_print("First operand data_type: %d\n", first_operand);
                d_print("Second operand data_type: %d\n", second_operand);

                /*int > int*/
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_G, op_1, op_2, tmp));

                /*int > double or double > int */
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_G, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_G, op_1, var, tmp));
                        }

                /*double > double*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_G, op_1, op_2, tmp));
                /* comparing string - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing 2 booleans - error*/
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing boolean with int or double - error*/
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                 /*comparing boolean with int or double - error*/
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /*E -> E >= E*/
        case P_GRE:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                /* int >= int*/
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_GE, op_1, op_2, tmp));
                /*int >= double or double >= int*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_GE, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_GE, op_1, var, tmp));
                        }

                /*double >= double*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_GE, op_1, op_2, tmp));
                /*comparing string - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                 /*comparing boolean with int or double - error*/
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                 /*comparing boolean with int or double - error*/
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing 2 booleans - error*/
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

        /*E -> E == E*/
        case P_EQL:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                /*int == int*/
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_E, op_1, op_2, tmp));
                /*int == double or double == int*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_E, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_E, op_1, var, tmp));
                        }

                /*double == double*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_E, op_1, op_2, tmp));
                /*comparing string - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing boolean and int or double */
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing boolean and int or double */
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing 2 booleans = error*/
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /*E -> E != E*/
        case P_NEQL:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                /*int != int*/
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_NE, op_1, op_2, tmp));
                /*int != double or double != int*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);
                        /*int operand conversion to double*/
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_NE, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_NE, op_1, var, tmp));
                        }

                /*comparing string = error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing boolean and int or double */
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing boolean and int or double */
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*comparing 2 booleans = error*/
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /* E -> E && E*/
        case P_AND:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_bol(op_1->b);
                d_bol(op_2->b);
                /* int && int - error*/
                if (first_operand == INT && second_operand == INT) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

                /*int && double or double && int*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

                /*double && double - error*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*string in logical expression - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean && boolean */
                } else if ((first_operand == BOOLEAN && second_operand == BOOLEAN)
                           || (second_operand == BOOLEAN && first_operand == BOOLEAN)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_AND, op_1, op_2, tmp));
                /*boolean && int or double - error*/
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*boolean && int or double - error*/
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;
        /*E-> E || E*/
        case P_OR:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                d_bol(op_1->b);
                d_bol(op_2->b);
               /* int || int - error*/
                if (first_operand == INT && second_operand == INT) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

                /* int || double or double || int - error*/
                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

                /* double || double - error*/
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*string in logical expression - error*/
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /* boolean || boolean*/
                } else if ((first_operand == BOOLEAN && second_operand == BOOLEAN)
                           || (second_operand == BOOLEAN && first_operand == BOOLEAN)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_OR, op_1, op_2, tmp));
                /* boolean || int or double - error*/
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /* boolean || int or double - error*/
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }
                result_item.expr = tmp;
                break;
        /*E -> !E*/
        case P_NOT:
                if (P->top->term != P_EXPR || P->top->LPtr->LPtr->term != P_HANDLE) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->value.data_type;
                op_1 = P->top->expr;
                d_bol(P->top->expr->b);
                /*!boolean*/
                if (first_operand == BOOLEAN) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_NOT, op_1, NULL, tmp));
                /*!int - error*/
                } else if (first_operand == INT) {
                        fprintf(stderr, "Incompatible data type.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*!double - error*/
                } else if (first_operand == DOUBLE) {
                        fprintf(stderr, "Incompatible data type.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                /*!string - error*/
                } else if (first_operand == STRING) {
                        fprintf(stderr, "Incompatible data type.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;

        default:
                d_message("Nastala chyba.\n");
                fprintf(stderr, "Unexpected expression.\n");
                expr_exit(SYNTACTIC_ANALYSIS_ERROR);
        }
        /*reducing nonterminals from stack*/
        while (P->top->term != P_HANDLE) {
                PSPop(P);
        }
        PSPop(P);
        /*push new item (result of previous rule) on top of a stack*/
        PSPush(P, P_EXPR);
        d_print("Term: %d\n", P->top->term);
        /*result data type and address of structure where is the result value is assigned to a nonterminal on the top*/
        P->top->value.data_type = result_item.value.data_type;
        P->top->expr = result_item.expr;
        d_bol(P->top->expr->b);

        return 1;
}

/*initialize nonterminal on the top of stack with address of structure where is the value of input token*/
int init_item(PStack * P, token_t * t) {

        PStack_item *push_item = P->top;
        symbol_table_item_t *item;

        switch (t->type) {

        case ID:
                /* id needs to be searched in the symbol table of function or class*/
                if (current_function.id_name != NULL) {
                        /*if we are in function, we need to check function symbol table*/
                        symbol_table_t *function = get_symbol_table_for_function(current_class,
                                                                                 current_function.id_name);
                        d_print("Current function is:%s\n", current_function.id_name);
                        d_print("premenna %s \n", t->string_value);
                        item = get_symbol_table_function_item(function, t->string_value);
                        /*we are using variable declared in class*/
                        if (item == NULL) {
                                item = get_symbol_table_class_item(current_class, t->string_value);
                        }

                        expr_var_name = t->string_value;

                        switch (item->variable.data_type) {
                        case INT:

                                push_item->value.data_type = INT;
                                push_item->expr = &item->variable;
                                d_int(push_item->expr->i);
                                break;
                        case DOUBLE:

                                push_item->value.data_type = DOUBLE;
                                push_item->expr = &item->variable;
                                d_dob(push_item->expr->d);
                                break;
                        case STRING:

                                push_item->value.data_type = STRING;
                                push_item->expr = &item->variable;
                                d_str(push_item->expr->s);
                                break;
                        case BOOLEAN:

                                push_item->value.data_type = BOOLEAN;
                                push_item->expr = &item->variable;
                                d_bol(push_item->expr->b);
                                break;
                        }

                } else {
                        /*we are in class*/
                        item = get_symbol_table_class_item(current_class, t->string_value);

                        switch (item->variable.data_type) {
                        case INT:

                                push_item->value.data_type = INT;
                                push_item->expr = &item->variable;
                                d_int(push_item->expr->i);

                                break;
                        case DOUBLE:

                                push_item->value.data_type = DOUBLE;
                                push_item->expr = &item->variable;
                                d_dob(push_item->expr->d);
                                break;
                        case STRING:

                                push_item->value.data_type = STRING;
                                push_item->expr = &item->variable;
                                d_str(push_item->expr->s);
                                break;
                        case BOOLEAN:

                                push_item->value.data_type = BOOLEAN;
                                push_item->expr = &item->variable;
                                d_bol(push_item->expr->b);
                                break;
                        }
                }


                break;
        case SPECIAL_ID:
                d_print("Value of literal is:%s\n", t->string_value);
                /*get symbol table for special id*/
                item = get_symbol_table_special_id_item(t->string_value);

                expr_var_name = t->string_value;

                d_print("data_type special id %d\n", item->variable.data_type);

                switch (item->variable.data_type) {
                case INT:

                        push_item->value.data_type = INT;
                        push_item->expr = &item->variable;
                        d_int(push_item->expr->i);
                        break;
                case DOUBLE:

                        push_item->value.data_type = DOUBLE;
                        push_item->expr = &item->variable;
                        d_dob(push_item->expr->d);
                        break;
                case STRING:

                        push_item->value.data_type = STRING;
                        push_item->expr = &item->variable;
                        d_str(push_item->expr->s);
                        break;
                case BOOLEAN:

                        push_item->value.data_type = BOOLEAN;
                        push_item->expr = &item->variable;
                        d_bol(push_item->expr->b);
                        break;
                }


                break;


        case INT_LITERAL:

                push_item->value.i = t->int_value;
                push_item->value.data_type = INT;
                /*int constant is inserted to a list of constants*/
                tVar *i = insert_int_const(&mem_constants, t->int_value);
                push_item->expr = i;
                break;
        case DOUBLE_LITERAL:

                push_item->value.d = t->double_value;
                push_item->value.data_type = DOUBLE;
                /*double constant is inserted to a list of constants*/
                tVar *d = insert_double_const(&mem_constants, t->double_value);
                push_item->expr = d;
                break;
        case STRING_LITERAL:

                push_item->value.s = t->string_value;
                push_item->value.data_type = STRING;
                /*string constant is inserted to a list of constants*/
                tVar *s = insert_string_const(&mem_constants, t->string_value);
                push_item->expr = s;
                break;
        case TRUE:

                push_item->value.b = true;
                push_item->value.data_type = BOOLEAN;
                /*boolean constant is inserted to a list of constants*/
                tVar *bt = insert_boolean_const(&mem_constants, true);
                push_item->expr = bt;
                break;
        case FALSE:

                push_item->value.b = false;
                push_item->value.data_type = BOOLEAN;
                /*boolean constant is inserted to a list of constants*/
                tVar *bf = insert_boolean_const(&mem_constants, false);
                push_item->expr = bf;
                break;

        }

        return 1;
}

/*generating tmp variable used in generating instructions, it is stored in symbol table*/
tVar *generate_tmp_var(int data_type) {
        if (current_function.id_name != NULL) {
                top_expr_variable = &insert_tmp_variable_symbol_table_function(current_function.id_name, data_type)->variable;
        } else {
                top_expr_variable = &insert_tmp_variable_symbol_table_class(data_type)->variable;
        }

        return top_expr_variable;
}
/*checks expression in ifj16print function*/
void check_expr_ifj_print(token_buffer_t * buffer, int expr_end_data_type) {
        if (buffer == NULL) {
                return;
        }
        if (expr_end_data_type != STRING && expr_len > 1) {
                fprintf(stderr, "Invalid expression in ifj16.print\n");
                exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
        }
        for (int i = 0; i < buffer->length; i++) {
                switch (buffer->list[i]->type) {
                case NEG:
                case SUB:
                case MUL:
                case DIV:
                case OR:
                case AND:
                case LEFT_ROUNDED_BRACKET:
                case RIGHT_ROUNDED_BRACKET:
                        fprintf(stderr, "Invalid expression in ifj16.print\n");
                        exit(SYNTACTIC_ANALYSIS_ERROR);
                        break;

                default:
                        /* okay in print */
                        break;
                }
        }
}

/*PSA function*/
int get_psa(token_buffer_t * buffer, symbol_table_item_t * st_item, tVar ** expr_result) {
        expr_token_buffer = buffer;
        expr_len = buffer->length;
        expr_in_function = false;
        d_print("class %s\n", current_class);
        if (current_function.id_name != NULL) {
                d_print("vo funkcii %s\n", current_function.id_name);
                symbol_table_item_t *function = get_symbol_table_class_item(current_class,current_function.id_name);
                /*set current instruction tape to function tape*/
                work_tape = function->function.instruction_tape;
                expr_in_function = true;
        } else {
                /*actual tape is global tape*/
                work_tape = global_inst_tape;

        }
        int psa_result;
        token_t *t = NULL;
        token_t end;
        /*endmark token needs to be placed at the end of token buffer, then we will know when to stop analysis*/
        end.type = ENDMARK;
        add_token_to_buffer(buffer, &end);

        PStack *P = PSInit();
        PSPush(P, P_ENDMARK);

        expr_buffer_position = 0;
        d_print("in PSO %d:\n", buffer->length);

        t = get_next_token_psa(buffer);
        do {


                int r = PSTopTerm(P);
                switch (decode_table(r, decode_token_array[t->type])) {

                case '=':
                        PSPush(P, decode_token_array[t->type]);
                        t = get_next_token_psa(buffer);
                        break;
                case '<':
                        insert_handle(P, PSTopTermPtr(P));
                        PSPush(P, decode_token_array[t->type]);
                        init_item(P, t);
                        t = get_next_token_psa(buffer);

                        break;

                case '>':

                        choose_rule(P);

                        break;
                case ' ':
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                        break;



                }


        } while (PSTopTerm(P) != P_ENDMARK || t->type != ENDMARK);

        st_item->id_name = "expr_result";
        st_item->variable = *P->top->expr;
        st_item->variable.data_type = P->top->value.data_type;
        *expr_result = P->top->expr;

        if (function_name_call != NULL && strcmp(function_name_call, "ifj16.print") == 0) {
                check_expr_ifj_print(buffer, P->top->value.data_type);
        }

        d_print("TOP (dollar) term je : %d\n", PSTopTerm(P));
        d_print("TOP term je : %d\n", PSTopTermPtr(P)->RPtr->term);
        if (PSTopTermPtr(P)->RPtr->RPtr == NULL) {
                d_message("Uspech\n");
        }
        d_message("Na konci:\n");
        PSPrint(P);
        d_print("Data type of result is:%d\n", P->top->value.data_type);
        psa_result = P->top->value.data_type;
        PSDispose(P);
        return psa_result;


}
