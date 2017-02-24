/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Martin Marušiak, xmarus07
 */

#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "instructions.h"
#include "debug.h"
#include "error_codes.h"

#define likely(x)       __builtin_expect((x),1)

// prints error message
#define pr_er fprintf(stderr, "Using uninitialized variable\n")
//this makto initialize variable
#define initialize(var) (var != NULL) ? (var->initialized = true, var) : (NULL)
//checks if the operand is initialized
#define check_init(var) (var->initialized) ? (var) : (pr_er, exit(RUN_UNINITIALIZED_VARIABLE_ERROR), NULL)
//evaluates the adress of results - gets effective adress of result
#define evaluate_res(adress) ((adress != NULL) ? (get_e_adr(adress)) : (NULL))
//evaluates the operands - gets their effective adress and check them for initialization
#define evaluate_op(adress) ((adress != NULL) ? (check_init(get_e_adr(adress))) : (NULL))
//gets effective adress of variable
#define get_e_adr(adress) \
        ((adress->offset == CONSTANT) ? (adress) : (adress->offset + frame_stack.top->frame->local))

//max size of stack buffer
#define STACK_BUFFER_SIZE 32
//max size of frame buffer
#define FRAME_BUFFER_SIZE 9

//instruction tape that is actually being processed
tList * processed_tape;

// buffer used for frames
tFrame * frame_buffer[FRAME_BUFFER_SIZE];
// is used to store size of frame buffer - number of frames in frame buffer
int frame_buf_size;
// buffer for stack
tFSElem stack_buffer[STACK_BUFFER_SIZE];
// size of stack - number of frames that are on stack
int stack_size;

//frame initialization
tFrame * init_frame(unsigned size){

        tFrame * new_frame;
        int i = 0;

        /* frames buffer - recycling frames, the last item of array is stop*/
        while(frame_buffer[i] != NULL){
            if(frame_buffer[i]->size >= size){
                new_frame = frame_buffer[i];
                new_frame->ret_val = NULL;
                frame_buf_size -= 1;
                frame_buffer[i] = frame_buffer[frame_buf_size];
                frame_buffer[frame_buf_size] = NULL;
                return new_frame;
            }
            i++;
        }
        //if there isn't suitable frame in frame buffer we will create new frame
        if((new_frame = malloc(sizeof(tFrame) + size*sizeof(tVar))) == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }

        new_frame->ret_val = NULL;
        new_frame->size = size;


        return new_frame;
}
//disposes all frames from stack and buffer
void dispose_frame_buffer(){
    while(frame_stack.top != NULL){
        pop_frame(&frame_stack);
    }

    for(int i=0; i < FRAME_BUFFER_SIZE-1; i++)
        if(frame_buffer[i] != NULL){
           free(frame_buffer[i]);
        }
}
//initializes stack of frames
void init_frame_stack(tFrameStack *stack){
        stack->top = NULL;
        stack->prepared = NULL;
}
//returns top frame
tFrame * top_frame(tFrameStack *stack){
        return stack->top->frame;
}
//pushes frame on the top of frame stack
void push_frame(tFrameStack *stack, tFrame * frame){

        tFSElem *ptr;
        if(stack_size >= STACK_BUFFER_SIZE){
            if((ptr = malloc(sizeof(tFSElem))) == NULL) {
                    exit(INTERNAL_INTERPRET_ERROR);
            }
        }
        else{
            //preallocated buffer
            ptr = &stack_buffer[stack_size];
        }
        stack_size++;
        ptr->frame = frame;
        ptr->next = stack->top;
        stack->top = ptr;
}
//pops the frame from stack of frames
void pop_frame(tFrameStack *stack){

        tFSElem *tmp;

        if(stack->top != NULL) {
                tmp = stack->top;
                /* change the stack top */
                stack->top = stack->top->next;
                /* free the strings allocated "in" frame */
                for(int i = tmp->frame->size; i--;) {
                        if(tmp->frame->local[i].data_type == STRING) {
                                if(tmp->frame->local[i].initialized){
                                    free(tmp->frame->local[i].s);
                                    tmp->frame->local[i].initialized = false;
                                }
                        }
                }
                /* adding poped frame to frame buffer (the last one item will always be NULL - the stop)*/
                if(frame_buf_size < FRAME_BUFFER_SIZE - 1){
                    frame_buffer[frame_buf_size++] = tmp->frame;
                }
                else{//frame buffer is full, freeing first frame in frame buffer and adding poped frame to frame buffer
                    free(frame_buffer[0]);
                    frame_buffer[0] = tmp->frame;
                }
                //if stack size is greater than max buffer size, item was allocated on heap so we need to free it
                if(stack_size > STACK_BUFFER_SIZE)
                    free(tmp);
                stack_size--;
        }
}

//processes 3-adress code
void interpret_tac(tList *inst_tape){
        d_message("processing new instruction tape");
        First_M(inst_tape); //setting active instruction to first instruction in instruction tape
        d_message("beginning of tape");
        tVar *op1; //first operand
        tVar *op2; //second operand
        tVar *result; //result
        tInst * inst; //operator - pointer to function
        //going through instruction tape
        while(Active_M(inst_tape)) {
                Copy_M(inst_tape, inst); //gets active instruction from instruction tape
                d_message("processing adresses");
                op1 = evaluate_op(inst->op1);
                op2 = evaluate_op(inst->op2);
                result = evaluate_res(inst->result);
                d_message("processing instruction");
                d_tVarPtr(op1);//prints value and adress of variable
                d_tVarPtr(op2);
                inst->f(op1, op2, result);
                initialize(result);//initializing result
                d_tVarPtr(result);
                d_message("instruction was processed");
                Succ_M(inst_tape);//sets activity to next instruction
        }

        return ;
}
