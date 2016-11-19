#ifndef INTERPRET_H
#define INTERPRET_H
#include "DLList.h"
#include "symbol_table.h"

typedef struct tFrame{       
    tVar *ret_val;
    int size;
    tVar local[]; //flexible array member    
}tFrame;

typedef struct tFSElem {
    tFrame *frame;
    struct tFSElem * next;
} tFSElem;

typedef struct tFrameStack{
    tFSElem *top;
    tFrame *prepared;
} tFrameStack;
    


extern tFrameStack frame_stack;
extern int push_counter;
extern tDLList * processed_tape;

int interpret_tac(tDLList *inst_tape);

void init_frame_stack(tFrameStack *stack);
tFrame * top_frame(tFrameStack *stack);
void pop_frame(tFrameStack *stack);
void push_frame(tFrameStack *stack, tFrame * frame);
void dispose_frame(tFrame *frame);
tFrame * init_frame(unsigned size);

void set_effective_adresess(tDLList *inst_tape);

#endif //INTERPRET
