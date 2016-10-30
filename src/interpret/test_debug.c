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
    d_message("test:");*/

    tDLList L;
    DLInitList(&L, dispose_inst2);
   
    /**
     * result = 23 + 10
     * result = 23 - 10 
     * result = 23 * 10
     * result = 23 / 10
     * if(10 > 5)
     *      x = 15;
     * else
     *      x = 42;     
     */    

        
    /*testovanie aritmetickych instrukcii*/ 
    tInst_fun *af[] = {i_add, i_sub, i_mul, i_div};
    
    for(unsigned j=0; j < 4; j++){
        tInst *i = init_inst2();

        i->op1->i = 23;
        i->op2->i = 10;
        i->f = af[j];
        DLInsertLast(&L, i);
    }    
    
    // 10 > 5
    tInst *i = init_inst2();
    i->op1->i = 10;
    i->op2->i = 5;
    i->f = i_g; //less
    DLInsertLast(&L, i);
    //jump 
 //   tInst *j = init_inst2();
//    j->f = i_jc;
//    j->op1->b = false;    
//    j->op2 = &L;
//    DLInsertLast(&L, j);
    //assign1
    i = init_inst2();
    i->f = i_assign;
    i->op1->i = 15;
    DLInsertLast(&L, i);

    //label
    i = init_inst2();
    i->f = i_label; 
    DLInsertLast(&L, i);
    //seting adress to label
//    DLLast(&L);
   // j->result = DLActiveElem(&L);

    //assign2
    i = init_inst2();
    i->f = i_assign;
    i->op1->i = 42;
    DLInsertLast(&L, i);

    interpret_tac(&L);
    DLDisposeList(&L);
    
    return 0;
}
