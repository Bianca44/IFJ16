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


#define SIZE 18


extern char *current_class;
extern symbol_table_item_t current_function;
extern tList *global_inst_tape;
extern constant_t *mem_constants;


token_buffer_t *expr_token_buffer;


tVar *top_expr_variable;

tList *work_tape;
tVar *op_1;
tVar *op_2;
tVar *tmp;
tVar *var = NULL;
char precedence_table[SIZE][SIZE] = {
/* input token *
        /*  +   -   *   /   (   )   <   >  <=  >=  ==   !=  &&  ||  id  lit  $   ! */ /* stack top */
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

int expr_check(PStack * P) {
        if (P->top->term != P_EXPR || P->top->LPtr->LPtr->term != P_EXPR) {
                return 0;
        }
        return 1;
}

void expr_exit(int exit_code) {
        free_token_buffer_local(expr_token_buffer);
        //PSDispose(P);
        exit(exit_code);
}

char decode_table(int top_term, int input_token) {


        return precedence_table[top_term][input_token];

}

int buffer_position = 0;
token_t *get_next_token_psa(token_buffer_t * token_buf) {
        if (buffer_position < token_buf->length) {
                return token_buf->list[buffer_position++];
        } else {
                return NULL;
        }
}

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
                        result_item.value.data_type = INT; // TODO odmazat potom
                        d_int(top_item->expr->i);
                        result_item.expr = top_item->expr;
                        //result_item.is_constant = true;
                        break;
                case DOUBLE:
                        result_item.value.data_type = DOUBLE;
                        d_dob(top_item->expr->d);
                        result_item.expr = top_item->expr;
                        //result_item.is_constant = true;
                        break;
                case STRING:
                        result_item.value.data_type = STRING;
                        d_str(top_item->expr->s);
                        result_item.expr = top_item->expr;
                        //result_item.is_constant = true;
                        break;
                case BOOLEAN:
                        result_item.value.data_type = BOOLEAN;
                        d_bol(top_item->expr->b);
                        result_item.expr = top_item->expr;
                        //result_item.is_constant = true;
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


                        InsertLast(work_tape, generate(I_ADD, op_1, op_2, tmp));
                        // }
                } /* int + double or double + int */
                else if ((first_operand == INT && second_operand == DOUBLE)
                         || (second_operand == INT && first_operand == DOUBLE)) {

                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_ADD, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_ADD, op_1, var, tmp));
                        }

                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_ADD, op_1, op_2, tmp));

                } else if (first_operand == STRING
                           && (second_operand == INT || second_operand == DOUBLE || second_operand == BOOLEAN)) {
                        // d_int(P->top->expr->i);
                        result_item.value.data_type = STRING;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_TO_STRING, op_2, NULL, var));
                        InsertLast(work_tape, generate(I_CAT, op_1, var, tmp));


                } else if (second_operand == STRING
                           && (first_operand == INT || first_operand == DOUBLE || first_operand == BOOLEAN)) {

                        result_item.value.data_type = STRING;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_TO_STRING, op_1, NULL, var));
                        InsertLast(work_tape, generate(I_CAT, var, op_2, tmp));
                } else if (first_operand == STRING && second_operand == STRING) {
                        result_item.value.data_type = STRING;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_CAT, op_1, op_2, tmp));
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

                if (first_operand == INT && second_operand == INT) {

                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);

                        InsertLast(work_tape, generate(I_SUB, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (second_operand == INT && first_operand == DOUBLE)) {
                        result_item.value.data_type = DOUBLE;

                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);
                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_SUB, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_SUB, op_1, var, tmp));
                        }
                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_SUB, op_1, op_2, tmp));

                } else if (first_operand == STRING || second_operand == STRING) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);

                } else if (first_operand == BOOLEAN || second_operand == BOOLEAN) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /* E -> E * E */
        case P_MUL:
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
                if (first_operand == INT && second_operand == INT) {

                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_MUL, op_1, op_2, tmp));



                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (second_operand == INT && first_operand == DOUBLE)) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_MUL, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_MUL, op_1, var, tmp));
                        }

                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_MUL, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN || second_operand == BOOLEAN) {
                        printf("Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        /* E -> E / E */
        case P_DIV:
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

                if (first_operand == INT && second_operand == INT) {


                        //zatial sa nekontroluje delenie 0
                        result_item.value.data_type = INT;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_DIV, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (second_operand == INT && first_operand == DOUBLE)) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(result_item.value.data_type);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_DIV, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_DIV, op_1, var, tmp));
                        }

                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = DOUBLE;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_DIV, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
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
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_L, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_L, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_L, op_1, var, tmp));
                        }


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_L, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                } else if ((first_operand == BOOLEAN && second_operand == BOOLEAN)
                           || (second_operand == BOOLEAN && first_operand == BOOLEAN)) {
                        result_item.value.data_type = BOOLEAN;
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                        //TODO nemozeme porovnavat 2 bool hodnoty
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;

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
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_LE, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_LE, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_LE, op_1, var, tmp));
                        }


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_LE, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;

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
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_G, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_G, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_G, op_1, var, tmp));
                        }


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_G, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        case P_GRE:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_GE, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_GE, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_GE, op_1, var, tmp));
                        }


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_GE, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;


        case P_EQL:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_E, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_E, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_E, op_1, var, tmp));
                        }


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_E, op_1, op_2, tmp));
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
        case P_NEQL:
                if (!expr_check(P)) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->LPtr->LPtr->value.data_type;
                second_operand = P->top->value.data_type;
                op_1 = P->top->LPtr->LPtr->expr;
                op_2 = P->top->expr;
                if (first_operand == INT && second_operand == INT) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_NE, op_1, op_2, tmp));

                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        var = generate_tmp_var(DOUBLE);

                        if (first_operand == INT) {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_1, NULL, var));
                                InsertLast(work_tape, generate(I_NE, var, op_2, tmp));
                        } else {
                                InsertLast(work_tape, generate(I_CONV_I_TO_D, op_2, NULL, var));
                                InsertLast(work_tape, generate(I_NE, op_1, var, tmp));
                        }


                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == BOOLEAN && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;

                break;
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

                if (first_operand == INT && second_operand == INT) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);


                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == BOOLEAN && second_operand == BOOLEAN)
                           || (second_operand == BOOLEAN && first_operand == BOOLEAN)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_AND, op_1, op_2, tmp));
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }

                result_item.expr = tmp;
                break;

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
                if (first_operand == INT && second_operand == INT) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);


                } else if ((first_operand == INT && second_operand == DOUBLE)
                           || (first_operand == DOUBLE && second_operand == INT)) {

                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);


                } else if (first_operand == DOUBLE && second_operand == DOUBLE) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == STRING || second_operand == STRING) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == BOOLEAN && second_operand == BOOLEAN)
                           || (second_operand == BOOLEAN && first_operand == BOOLEAN)) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_OR, op_1, op_2, tmp));
                } else if (first_operand == BOOLEAN && (second_operand == DOUBLE || second_operand == INT)) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if ((first_operand == INT || first_operand == DOUBLE)
                           && second_operand == BOOLEAN) {
                        fprintf(stderr, "Incompatible data types.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                }
                result_item.expr = tmp;
                break;
        case P_NOT:
                if (P->top->term != P_EXPR || P->top->LPtr->LPtr->term != P_HANDLE) {
                        fprintf(stderr, "Unexpected expression.\n");
                        expr_exit(SYNTACTIC_ANALYSIS_ERROR);
                }
                first_operand = P->top->value.data_type;
                op_1 = P->top->expr;
                d_bol(P->top->expr->b);
                if (first_operand == BOOLEAN) {
                        result_item.value.data_type = BOOLEAN;
                        tmp = generate_tmp_var(result_item.value.data_type);
                        InsertLast(work_tape, generate(I_NOT, op_1, NULL, tmp));
                } else if (first_operand == INT) {
                        fprintf(stderr, "Incompatible data type.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                } else if (first_operand == DOUBLE) {
                        fprintf(stderr, "Incompatible data type.\n");
                        expr_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
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
        while (P->top->term != P_HANDLE) {
                PSPop(P);
        }
        PSPop(P);
        PSPush(P, P_EXPR);
        d_print("Term: %d\n", P->top->term);

        P->top->value.data_type = result_item.value.data_type;
        P->top->expr = result_item.expr;
        d_bol(P->top->expr->b);

        return 1;
}


int init_item(PStack * P, token_t * t) {

        PStack_item *push_item = P->top;
        symbol_table_item_t *item;

        switch (t->type) {

        case ID:

                if (current_function.id_name != NULL) {

                        symbol_table_t *function = get_symbol_table_for_function(current_class,
                                                                                 current_function.id_name);
                        d_print("Current function is:%s\n", current_function.id_name);
                        d_print("premenna %s \n", t->string_value);
                        item = get_symbol_table_function_item(function, t->string_value);
                        if (item == NULL) {
                                item = get_symbol_table_class_item(current_class, t->string_value);
                        }

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

                item = get_symbol_table_special_id_item(t->string_value);

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

                tVar *i = insert_int_const(&mem_constants, t->int_value);
                push_item->expr = i;
                //push_item->is_constant = true;
                //d_bol(push_item->is_constant);
                break;
        case DOUBLE_LITERAL:

                push_item->value.d = t->double_value;
                push_item->value.data_type = DOUBLE;

                tVar *d = insert_double_const(&mem_constants, t->double_value);
                push_item->expr = d;
                //push_item->is_constant = true;
                //d_bol(push_item->is_constant);
                break;
        case STRING_LITERAL:

                push_item->value.s = t->string_value;
                push_item->value.data_type = STRING;

                tVar *s = insert_string_const(&mem_constants, t->string_value);
                push_item->expr = s;
                //push_item->is_constant = true;
                //d_bol(push_item->is_constant);
                break;
        case TRUE:

                push_item->value.b = true;
                push_item->value.data_type = BOOLEAN;

                tVar *bt = insert_boolean_const(&mem_constants, true);
                push_item->expr = bt;
                //push_item->is_constant = true;
                //d_bol(push_item->is_constant);
                break;
        case FALSE:

                push_item->value.b = false;
                push_item->value.data_type = BOOLEAN;

                tVar *bf = insert_boolean_const(&mem_constants, false);
                push_item->expr = bf;
                //push_item->is_constant = true;
                //d_bol(push_item->is_constant);
                break;

        }

        return 1;
}

tVar *generate_tmp_var(int data_type) {
        if (current_function.id_name != NULL) {
                top_expr_variable = &insert_tmp_variable_symbol_table_function(current_function.id_name, data_type)->variable;
        } else {
                top_expr_variable = &insert_tmp_variable_symbol_table_class(data_type)->variable;
        }

        return top_expr_variable;
}

int get_psa(token_buffer_t * buffer, symbol_table_item_t * st_item, tVar ** expr_result) {
        expr_token_buffer = buffer;

        d_print("class %s\n", current_class);
        if (current_function.id_name != NULL) {
                d_print("vo funkcii %s\n", current_function.id_name);
                symbol_table_item_t *function = get_symbol_table_class_item(current_class,
                                                                            current_function.id_name);
                work_tape = function->function.instruction_tape;

        } else {
                work_tape = global_inst_tape;

        }
        int psa_result;
        token_t *t = NULL;
        token_t end;
        end.type = ENDMARK;
        add_token_to_buffer(buffer, &end);

        PStack *P = PSInit();
        PSPush(P, P_ENDMARK);

        buffer_position = 0;
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
        *expr_result = P->top->expr;

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
