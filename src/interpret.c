#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "instructions.h"
#include "debug.h"
#include "error_codes.h"
#include "parser.h"

#define pr_er \
                    fprintf(stderr, "Using uninitialized variable\n")\

#define initialize(var) (var != NULL) ? (var->initialized = true, var) : (NULL)
#define check_init(var) (var->initialized) ? (var) : (pr_er,cleanup_exit(RUN_UNINITIALIZED_VARIABLE_ERROR), NULL)
#define evaluate_res(adress) ((adress != NULL) ? (get_e_adr(adress)) : (NULL))
#define evaluate_op(adress) ((adress != NULL) ? (check_init(get_e_adr(adress))) : (NULL))
#define get_e_adr(adress) \
    ((adress->offset == CONSTANT) ? (adress) : (adress->offset + frame_stack.top->frame->local))

//todo inicializovane ? assign, call_f

int push_counter;
tDLList * processed_tape;

tFrame * init_frame(unsigned size){

    tFrame * new_frame;
    if((new_frame = malloc(sizeof(tFrame) + size*sizeof(tVar))) == NULL){
        exit(INTERNAL_INTERPRET_ERROR);
    }

    new_frame->ret_val = NULL;
    new_frame->size = size;

    return new_frame;
}

void dispose_frame(tFrame *frame){
    free(frame);
}

void init_frame_stack(tFrameStack *stack){
    stack->top = NULL;
    stack->prepared = NULL;
}

tFrame * top_frame(tFrameStack *stack){
    return stack->top->frame;
}

void push_frame(tFrameStack *stack, tFrame * frame){

    tFSElem *ptr;
    if((ptr = malloc(sizeof(tFSElem))) == NULL){
        exit(INTERNAL_INTERPRET_ERROR);
    }

    ptr->frame = frame;
    ptr->next = stack->top;
    stack->top = ptr;
}

void pop_frame(tFrameStack *stack){

    tFSElem *tmp;

    if(stack->top != NULL){
        tmp = stack->top;
        //change stack top
        stack->top = stack->top->next;
        //free frame
        //TODO dalo by sa optimalizovat
        if(tmp->frame == NULL){
            d_message("PIL SI");
            exit(42);
        }
        for(int i = 0; i < tmp->frame->size; i++){
            if(tmp->frame->local[i].data_type == STRING){
                free(tmp->frame->local[i].s);
            }
        }
        dispose_frame(tmp->frame);
        free(tmp);
    }
}

int interpret_tac(tDLList *inst_tape){
    d_message("vykonanie novej pasky");
    DLFirst(inst_tape);
    d_message("skok na zaciatok pasky");
    tVar *op1, *op2, *result;
    tInst * inst;
    while(DLActive(inst_tape)){

        DLCopy(inst_tape, (void **)&inst);

        d_message("spracovanie adries");
        op1 = evaluate_op(inst->op1);
        op2 = evaluate_op(inst->op2);
        result = evaluate_res(inst->result);
        d_message("vykonanie instrukcie");
        d_tVarPtr(op1);
        d_tVarPtr(op2);
        inst->f(op1, op2, result);
        initialize(result);
        d_tVarPtr(result);
        d_message("instrukcia bola vykonana");
        DLSucc(inst_tape);
    }

   return 0; //TODO
}
