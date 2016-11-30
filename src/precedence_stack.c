/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Juraj Ondrej Dúbrava, xdubra03
 */
#include <stdio.h>
#include "precedence_stack.h"
#include "token_buffer.h"
#include "debug.h"
#include "error_codes.h"

char *prec_names[20] = { "P_ADD", "P_SUB", "P_MUL", "P_DIV", "P_LB", "P_RB",	// )
    "P_LESS", "P_GRT", "P_LESSE", "P_GRE", "P_EQL", "P_NEQL",	// !=
    "P_AND", "P_OR", "P_ID", "P_LIT", "P_ENDMARK", "P_NOT", "P_EXPR", "P_HANDLE"
};

/*return pointer to a newly created stack*/
PStack *PSInit() {

    PStack *P = malloc(sizeof(PStack));
    if (P == NULL) {
	fprintf(stderr,"Unable to allocate memory.\n");
    exit(INTERNAL_INTERPRET_ERROR);
    }
    P->top = NULL;
    P->first = NULL;

    return P;
}
/*push stack item on the top*/
void PSPush(PStack * P, enum Terminals term) {

    PStack_item *term_item;
    term_item = malloc(sizeof(PStack_item));
    if (term_item == NULL) {
	fprintf(stderr,"Unable to allocate memory.\n");
    exit(INTERNAL_INTERPRET_ERROR);
    }
    term_item->is_constant = false;
    term_item->expr = NULL;
    term_item->term = term;
    term_item->LPtr = P->top;
    term_item->RPtr = NULL;

    if (P->top != NULL) {
	P->top->RPtr = term_item;
    }
    if (P->first == NULL) {
	P->first = term_item;
    }
    P->top = term_item;
}
/*pop out the stack item*/
void PSPop(PStack * P) {

    if (P->top != NULL) {

	PStack_item *tmp = P->top;
	P->top = tmp->LPtr;
	P->top->RPtr = NULL;
	free(tmp);
    }
}
/*returns the top terminal on the stack*/
int PSTopTerm(PStack * P) {

    PStack_item *top_term = P->top;
    
    if (top_term->term <= P_NOT) {
	return top_term->term;
    } else {			
	top_term = top_term->LPtr;
	if (top_term->term <= P_NOT) {
	    return top_term->term;
	}
	while (!is_top_terminal(top_term->term)) {
	    if (top_term->LPtr != NULL) {
		top_term = top_term->LPtr;
	    }
	    
	}
	return top_term->term;
    }
    return 42;
}
/*returns the pointer to top terminal item*/
PStack_item *PSTopTermPtr(PStack * P) {

    PStack_item *top_term = P->top;
    if (top_term->term <= P_NOT) {
	return top_term;
    } else {			
	top_term = top_term->LPtr;
	while (!is_top_terminal(top_term->term)) {
	    top_term = top_term->LPtr;
	    if (top_term == P->first)
		return NULL;	
	}
	return top_term;
    }
    return NULL;
}
/*insert handle after terminal */
void insert_handle(PStack * P, PStack_item * item) {

    PStack_item *tmp;
    tmp = item->RPtr;
    PStack_item *handle_item;
    handle_item = malloc(sizeof(PStack_item));
    if (handle_item == NULL) {
	fprintf(stderr,"Unable to allocate memory.\n");
    exit(INTERNAL_INTERPRET_ERROR);
    }
    handle_item->term = P_HANDLE;
    handle_item->RPtr = item->RPtr;
    handle_item->LPtr = item;
    if (item->RPtr == NULL) {
	item->RPtr = handle_item;
	P->top = handle_item;
    } else {
	tmp->LPtr = handle_item;
	item->RPtr = handle_item;
    }

}
/*disposing the stack*/
void PSDispose(PStack * P) {
    
    PStack_item *tmp;
    while (P->top != NULL) {
	tmp = P->top;
	P->top = tmp->LPtr;
	free(tmp);
    }
    free(P);
}
/*checks if the item on the stack is terminal*/
bool is_top_terminal(int term) {
    if (term <= P_NOT) {
	return true;
    } else
	return false;
}
/*print the items of stack*/
void PSPrint(PStack * P) {

    if (P->first == NULL) {
	printf("Je null");
    }
    
    PStack_item *tmp = P->top;
    while (tmp != NULL) {
	d_print("Prvok zasobnika je: %s\n", prec_names[tmp->term]);
	tmp = tmp->LPtr;
    }
    return;

}
