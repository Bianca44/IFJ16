#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "datatypes.h"
#include "instructions.h"
#include "DLList.h"
#include "interpret.h"

int main(){
/*
    d_print("durko je %s","bilbo shwaggins");
    d_message("test:"); */

    
/*
    tVar x;
    x.i = 10;
    x.type = INT;
    x.initialized = true;
    tInst *i = init_inst();
    void *j;
    tVar *a[3];
    i->f = i_add;
    a[0] = i->op1 = malloc(sizeof(tVar));
    a[1] = i->op2 = malloc(sizeof(tVar));
    a[2] = i->result = malloc(sizeof(tVar));
    i->op1->i = 1;
    i->op2->i = 2;
    i->result->i = 3;

    i->f(&x,&x,&x);
    printf("%d %u %d \n", x.i, x.type, x.initialized);
    printf("%lu\n", sizeof(x));

    tDLList l;
    DLInitList(&l, dispose_inst);
    DLInsertFirst(&l, i);
    DLFirst(&l);
    DLCopy(&l, &j);
    ((tInst *)j)->f(((tInst *)j)->op1, ((tInst *)j)->op2, ((tInst *)j)->result);
    printf("%d %d %d %d\n", ((tInst *)j)->op1->i, ((tInst *)j)->op2->i, ((tInst *)j)->result->i,
    ((tInst *)j)->result->i);
    DLDisposeList(&l);   
    free(a[0]);
    free(a[1]);
    free(a[2]); 
*/
    return 0;
}
