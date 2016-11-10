#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "datatypes.h"
#include "instructions.h"
#include "DLList.h"
#include "interpret.h"


tFrameStack frame_stack;

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
    tInst *j = init_inst2();
    j->f = i_jnc;
    j->op1->b = true;
    j->op2->s = (char *)&L;
    DLInsertLast(&L, j);

    //assign1
    i = init_inst2();
    i->f = i_assign;
    i->op1->i = 12;
    DLInsertLast(&L, i);

    //goto
    tInst *k = init_inst2();
    k->f = i_goto;
    k->op2->s = (char *)&L;
    DLInsertLast(&L, k);

    //label
    i = init_inst2();
    i->f = i_label; 
    DLInsertLast(&L, i);
    //seting adress to label
    DLLast(&L);
    j->result->s = (char *)DLActiveElem(&L);
    
    //assign2
    i = init_inst2();
    i->f = i_assign;
    i->op1->i = 42;
    DLInsertLast(&L, i);
//label pre cyklus nizsie aj podm vyssie
    //label
    i = init_inst2();
    i->f = i_label; 
    DLInsertLast(&L, i);
    //seting adress to label
    DLLast(&L);
    k->result->s = (char *)DLActiveElem(&L);

//cyklus
    // 10 > 5
    i = init_inst2();
    i->op1->i = 100000000;
    i->op2->i = 0;
    i->f = i_g; //less
    tVar *p = i->result;
    DLInsertLast(&L, i);
    //jump 
    j = init_inst2();
    j->f = i_jnc;
    j->op1 = p;
    j->op2->s = &L;
    DLInsertLast(&L, j);

    //assign1
    i = init_inst2();
    i->f = i_assign;
    i->op1->i = 15;
    DLInsertLast(&L, i);

    //goto
    k = init_inst2();
    k->f = i_goto;
    k->op2->s = &L;
    k->result->s = DLActiveElem(&L);
    DLInsertLast(&L, k);


    //label end
    i = init_inst2();
    i->f = i_label; 
    DLInsertLast(&L, i);
    //seting adress to label
    DLLast(&L);
    j->result->s = DLActiveElem(&L);
   


//koniec cyklu
 
    //assign2
    i = init_inst2();
    i->f = i_assign;
    i->op1->i = 42;
    DLInsertLast(&L, i); 
    interpret_tac(&L);
    //DLDisposeList(&L);
    
    return 0;
}
