#ifndef INTERPRET_H
#define INTERPRET_H
#include "list.h"
#include "symbol_table.h"

typedef struct tFrame{       
    tVar *ret_val;
    int size;
    char *loc_types;
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
extern tList * processed_tape;

int interpret_tac(tList *inst_tape);

void init_frame_stack(tFrameStack *stack);
tFrame * top_frame(tFrameStack *stack);
void pop_frame(tFrameStack *stack);
void push_frame(tFrameStack *stack, tFrame * frame);
void dispose_frame(tFrame *frame);
tFrame * init_frame(unsigned size, char *);

void set_effective_adresess(tList *inst_tape);
void dispose_buffer();

#endif //INTERPRET
