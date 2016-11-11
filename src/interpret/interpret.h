#ifndef INTERPRET_H
#define INTERPRET_H
#include "DLList.h"
#include "datatypes.h"

int interpret_tac(tDLList *inst_tape);

void init_frame_stack(tFrameStack *stack);
tFrame * top_frame(tFrameStack *stack);
void pop_frame(tFrameStack *stack);
void push_frame(tFrameStack *stack, tFrame * frame);
void dispose_frame(tFrame *frame);
tFrame * init_frame(unsigned size);

#endif //INTERPRET
