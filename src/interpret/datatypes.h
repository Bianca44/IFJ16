#ifndef DATATYPES_H
#define DATATYPES_H
#include <stdbool.h>

enum data_type {
    INT,
    DOUBLE,
    STRING,
    BOOLEAN,
};

typedef struct tVar {
    enum data_type type;
    union {
        int i;
        double d;
        char *s;
        bool b;
    };
    bool initialized;
} tVar;

typedef void tInst_fun(tVar *op1, tVar *op2, tVar *result);

typedef struct tInst{
    tInst_fun *f;
    tVar *op1;
    tVar *op2;
    tVar *result;
} tInst;


tInst *init_inst();
void dispose_inst(void *);
#endif //INSTRUCTIONS_H
