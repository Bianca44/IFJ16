#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "instructions.h"
#include "debug.h"
#include "error_codes.h"


#define pr_er \
        fprintf(stderr, "Using uninitialized variable\n") \

#define initialize(var) (var != NULL) ? (var->initialized = true, var) : (NULL)
#define check_init(var) (var->initialized) ? (var) : (pr_er, exit(RUN_UNINITIALIZED_VARIABLE_ERROR), NULL)
#define evaluate_res(adress) ((adress != NULL) ? (get_e_adr(adress)) : (NULL))
#define evaluate_op(adress) ((adress != NULL) ? (check_init(get_e_adr(adress))) : (NULL))
#define get_e_adr(adress) \
        ((adress->offset == CONSTANT) ? (adress) : (adress->offset + frame_stack.top->frame->local))

#define STACK_BUF_SIZE 32
#define FRAME_BUFFER_SIZE 9

int push_counter;
tList * processed_tape;

tFrame * frame_buffer[FRAME_BUFFER_SIZE] = {NULL, };
int frame_buf_size;

tFSElem stack_buf[STACK_BUF_SIZE];// = {NULL, };
int stack_buf_size = 0;

tFrame * init_frame(unsigned size, char *loc_types){

        tFrame * new_frame;
        int i = 0;

        //recyklovanie ramcov
        while(frame_buffer[i] != NULL){
            if(frame_buffer[i]->size >= (int) size){
                new_frame = frame_buffer[i];
                new_frame->loc_types = loc_types;
                frame_buf_size -= 1;                
                frame_buffer[i] = frame_buffer[frame_buf_size];
                frame_buffer[frame_buf_size] = NULL;
                return new_frame;
            }
            i++;
        }        
          
        if((new_frame = malloc(sizeof(tFrame) + size*sizeof(tVar))) == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        
        
        new_frame->loc_types = loc_types;
        new_frame->ret_val = NULL;
        new_frame->size = size;

       
        return new_frame;
}
void dispose_buffer(){
    for(int i=0; i < FRAME_BUFFER_SIZE; i++)
        if(frame_buffer[i] != NULL)
            free(frame_buffer[i]);
}
/*
void dispose_frame(tFrame *frame){
        free(frame);
}
*/
void init_frame_stack(tFrameStack *stack){
        stack->top = NULL;
        stack->prepared = NULL;
}

tFrame * top_frame(tFrameStack *stack){
        return stack->top->frame;
}

void push_frame(tFrameStack *stack, tFrame * frame){

        tFSElem *ptr;
        if(stack_buf_size >= STACK_BUF_SIZE){
            if((ptr = malloc(sizeof(tFSElem))) == NULL) {
                    exit(INTERNAL_INTERPRET_ERROR);
            }
        }
        else{
            ptr = &stack_buf[stack_buf_size];
        }
        stack_buf_size++;
        ptr->frame = frame;
        ptr->next = stack->top;
        stack->top = ptr;
}

void pop_frame(tFrameStack *stack){

        tFSElem *tmp;

        if(stack->top != NULL) {
                tmp = stack->top;
                //change stack top
                stack->top = stack->top->next;
                //free frame
                //TODO dalo by sa optimalizovat
                if(tmp->frame == NULL) {
                        d_message("PIL SI");
                        exit(42);
                }
                int i;
                for(i = tmp->frame->size; i--;) {

                        if(tmp->frame->local[i].data_type == STRING) {
                                free(tmp->frame->local[i].s);
                        }
                }
                //dispose_frame(tmp->frame);
                //posledny neplnime bude vzdy NULL - zarazka
                
                if(frame_buf_size < FRAME_BUFFER_SIZE - 1){
                    frame_buffer[frame_buf_size++] = tmp->frame;
                }
                else{
                    free(frame_buffer[0]);
                    frame_buffer[0] = tmp->frame;
                    //free(tmp->frame);
                }
                if(stack_buf_size > STACK_BUF_SIZE)
                    free(tmp);
                stack_buf_size--;
        }
}

int interpret_tac(tList *inst_tape){
        d_message("vykonanie novej pasky");
        First_M(inst_tape);
        d_message("skok na zaciatok pasky");
        register tVar *op1;
        register tVar *op2;
        register tVar*result;
        register tInst * inst;
        while(Active_M(inst_tape)) {

                Copy_M(inst_tape, inst);
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
                Succ_M(inst_tape);
        }

        return 0; //TODO
}
