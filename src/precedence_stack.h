/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Juraj Ondrej Dúbrava, xdubra03
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "symbol_table.h"
#include "scanner.h"


enum Terminals {
    P_ADD,			// +
    P_SUB,			// -
    P_MUL,			// *
    P_DIV,			// /
    P_LB,			// (
    P_RB,			// )
    P_LESS,			// <
    P_GRT,			// >
    P_LESSE,		// <=
    P_GRE,			// >=
    P_EQL,			// ==
    P_NEQL,			// !=
    P_AND,			// &&
    P_OR,			// ||
    P_ID,			// id
    P_LIT,			// literal
    P_ENDMARK,		//$
    P_NOT,			// !
    P_EXPR,			// E
    P_HANDLE		// <
};






typedef struct PStack_item {
    struct PStack_item *LPtr;
    struct PStack_item *RPtr;
    /* type of the terminal */
    enum Terminals term;
    tVar value;
    /*pointer to variable which holds the value*/
    tVar *expr;
    bool is_constant;
} PStack_item;

typedef struct {
    PStack_item *top;
    PStack_item *first;

} PStack;



PStack *PSInit();
void PSPush(PStack * P, enum Terminals terms);
void PSPop(PStack * P);
/* finds the first terminal */
int PSTopTerm(PStack * P);
/* points to the terminal item on the top of the stack */
PStack_item *PSTopTermPtr(PStack * P);
void PSDispose(PStack * P);
bool is_top_terminal(int term);
void insert_handle(PStack * P, PStack_item * item);
void PSPrint(PStack * P);
