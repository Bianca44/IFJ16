#ifndef DATATYPES_H
#define DATATYPES_H
#include <stdbool.h>

enum data_type {
    INT,
    DOUBLE,
    STRING,
    BOOLEAN,
};

#define CONST -1

typedef struct tVar {
    enum data_type type;
    union {
        int i;
        double d;
        char *s;
        bool b;
    };
    bool initialized;
    int offset;
} tVar;

typedef void tInst_fun(tVar *op1, tVar *op2, tVar *result);

typedef struct tInst{
    tInst_fun *f;
    tVar *op1_st;
    tVar *op1;
    tVar *op2_st;
    tVar *op2;
    tVar *result_st;
    tVar *result;
} tInst;

typedef struct tFrame{       
    tVar *result;
    tVar *local[]; //flexible array member    
}tFrame;

typedef struct tFSElem {
    tFrame *frame;
    struct tFSElem * next;
} tFSElem;

typedef struct tFrameStack{
    tFSElem *top;
    tFSElem *prepared;
}tFrameStack;
    
extern tFrameStack frame_stack;

tInst *init_inst();
tInst *init_inst2();
void dispose_inst(void *);
void dispose_inst2(void *);
#endif //INSTRUCTIONS_H
