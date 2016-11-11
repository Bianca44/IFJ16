#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "instructions.h"
#include "DLList.h"
#include "interpret.h"
#include "symbol_table.h"
#include "ial.h"
#include "strings.h"
#include "scanner.h"

tFrameStack frame_stack;
char *current_class;
symbol_table_t *class_list;
symbol_table_item_t current_variable;
symbol_table_item_t function_variable;
symbol_table_item_t current_function;
string_t param_data_types;

tVar * get_adress(char *id, symbol_table_t *t){
   
    return &((symbol_table_item_t *)ht_read(t, id))->variable;
}

int main(){

    //filling symbol table for test purposes
    init_class_list();

    char * name = "Main";
    current_class = name;
    //set_current_class(name);
    insert_class(name);
    
    symbol_table_t * t = get_symbol_table_for_class(name);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));
    
    symbol_table_t * f = create_function_symbol_table();

    insert_function_variable_symbol_table(f, "par1", DOUBLE, 0);
    insert_function_variable_symbol_table(f, "par2", INT, 1);
    insert_function_variable_symbol_table(f, "a", INT, 1);
    insert_function_variable_symbol_table(f, "b", INT, 1);
    insert_function_variable_symbol_table(f, "c", INT, 1);

    insert_function_symbol_table("test", INT, 2, 3, "di", f);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));
   
    symbol_table_item_t *i = ht_read(f, "par1");
    d_print("%d", i->variable.data_type == DOUBLE);

    
    //insert(f->instrukcn, generate(ADD, adresu1, adresu2, adresu2));
    
    tDLList L;
    DLInitList(&L, dispose_inst);
    tVar pomocna;
    pomocna.i = 10; 
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna, NULL, get_adress("a",f)));
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna, NULL, get_adress("b",f)));
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna, NULL, get_adress("c",f)));
    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), get_adress("b",f), get_adress("c",f)));

    interpret_tac(&L);  
    DLDisposeList(&L);
    ht_free(class_list);


    
/*
    d_print("durko je %s","bilbo shwaggins");
    d_message("test:");

    tDLList L;
    DLInitList(&L, dispose_inst2);
 */
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

   /*     
    testovanie aritmetickych instrukcii
    tInst_fun *af[] = {i_add, i_sub, i_mul, i_div};
    
    for(unsigned j=0; j < 4; j++){
        tInst *i = init_inst2();

        i->op1->i = 23;
        i->op2->i = 10;
        i->f = af[j];
        DLInsertLast(&L, i);
    } */ 
/*    
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
    i->op1->i = 1;
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

    tFrameStack frame_stack;
    init_frame_stack(&frame_stack);
*/    
    return 0;
}
