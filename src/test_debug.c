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

int push_counter;
tDLList * processed_tape;
tVar * global_offset;

tFrameStack frame_stack;
symbol_table_t *class_list;
symbol_table_item_t current_variable;
symbol_table_item_t function_variable;
symbol_table_item_t current_function;
string_t param_data_types;

tVar * get_adress(char *id, symbol_table_t *t){
   
    return &((symbol_table_item_t *)ht_read(t, id))->variable;
}

void set_label(tDLElemPtr jump, tDLElemPtr where){
    //malloc a pridat offset v kombinacii s povedomim o vykonavanej paske
    //linearny zoznam
    //TODO
    tVar *new;
    if((new = malloc(sizeof(tVar))) == NULL){
        //TODO
    }
    new->offset = -1;
    new->initialized = true;
    new->s = (char *)where;
    ((tInst *)(jump->data))->result = new;
}
int main(){

    //filling symbol table for test purposes
    init_class_list();

    char * name = "Main";
    //set_current_class(name);
    insert_class(name);
    set_current_class(name);

    symbol_table_t * t = get_symbol_table_for_class(name);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));

    symbol_table_t * f = create_function_symbol_table();

    insert_function_variable_symbol_table(f, "par1", DOUBLE, -1);
    insert_function_variable_symbol_table(f, "par2", INT, -1);
    insert_function_variable_symbol_table(f, "a", INT, -1);
    insert_function_variable_symbol_table(f, "b", INT, -1);
    insert_function_variable_symbol_table(f, "c", INT, -1);
    insert_function_variable_symbol_table(f, "d", DOUBLE, -1);
    insert_function_variable_symbol_table(f, "e", DOUBLE, -1);
    insert_function_variable_symbol_table(f, "f", DOUBLE, -1);
    insert_function_variable_symbol_table(f, "bool", BOOLEAN, -1);

    insert_function_symbol_table("test", INT, 2, 3, "di", f);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));

    symbol_table_item_t *i = get_symbol_table_function_item(f, "par1");
    d_print("%d", i->variable.data_type == DOUBLE);


    //insert(f->instrukcn, generate(ADD, adresu1, adresu2, adresu2));

    tDLList L;
    DLInitList(&L, dispose_inst);

    tVar pomocna[100];
    for(int i=0 ;i < 100; i++){

        pomocna[i].offset = -1;
    }
    pomocna[0].i = 30;
    pomocna[1].i = 35; 
    pomocna[2].d = 10.2;
    pomocna[3].d = 2.57;
    pomocna[4].i = 1;

    DLInsertLast(&L, generate(I_ASSIGN, &pomocna[0], NULL, get_adress("a",f)));
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna[1], NULL, get_adress("b",f)));
    DLInsertLast(&L, generate(I_MUL, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
    DLInsertLast(&L, generate(I_SUB, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
    DLInsertLast(&L, generate(I_DIV, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
 
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna[2], NULL, get_adress("d",f)));
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna[3], NULL, get_adress("e",f)));
    DLInsertLast(&L, generate(I_MUL, get_adress("d",f), get_adress("e",f), get_adress("f",f)));
    DLInsertLast(&L, generate(I_ADD, get_adress("d",f), get_adress("e",f), get_adress("f",f)));
    DLInsertLast(&L, generate(I_SUB, get_adress("d",f), get_adress("e",f), get_adress("f",f)));
    DLInsertLast(&L, generate(I_DIV, get_adress("d",f), get_adress("e",f), get_adress("f",f)));

    
    //if(a > b)
    //{
    //  c = a*b
    //}
    //else
    //{
    //  c = a/b
    //}


    pomocna[5].offset = -1;
    pomocna[5].s = (char *)&L;
    d_message("pred prvym skokom");
    //tDLElemPtr inst;
    js_init();
    DLInsertLast(&L, generate(I_G, get_adress("a",f), get_adress("b",f), get_adress("bool",f)));
    //uloz na zosobnik
    DLInsertLast(&L, generate(I_JNT, get_adress("bool",f), NULL, NULL));
    js_push(DLGetLast(&L));
    DLInsertLast(&L, generate(I_MUL, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
    DLInsertLast(&L, generate(I_GOTO, NULL, NULL, NULL));
  
    d_message("pred nastavem labelu 1");
    set_label(js_top(), DLGetLast(&L));
    js_pop();
    js_push(DLGetLast(&L));
    DLInsertLast(&L, generate(I_DIV, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
  
    d_message("pred nastavem labelu 2");
    set_label(js_top(), DLGetLast(&L));
    js_pop();
    //

    d_message("prve skakanie if");

    DLInsertLast(&L, generate(I_ADD, get_adress("d",f), get_adress("e",f), get_adress("f",f)));
    DLInsertLast(&L, generate(I_SUB, get_adress("d",f), get_adress("e",f), get_adress("f",f)));


    //while(a<b){
    //  a = a + 1;
    //  }
    // instrukcna paska globalna premenna ? 
    js_push(DLGetLast(&L));
    DLInsertLast(&L, generate(I_LE, get_adress("a",f), get_adress("b",f), get_adress("bool",f)));
    DLInsertLast(&L, generate(I_JNT, get_adress("bool",f), NULL, NULL));
    js_push(DLGetLast(&L));
    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), &pomocna[4], get_adress("a",f)));
    DLInsertLast(&L, generate(I_GOTO, NULL, NULL, NULL));
    set_label(js_top(), DLGetLast(&L));
    js_pop();
    set_label(DLGetLast(&L), js_top());
    js_pop();
 

    DLInsertLast(&L, generate(I_SUB, get_adress("d",f), get_adress("e",f), get_adress("f",f)));
    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), &pomocna[4], get_adress("a",f)));
    

    tDLList F;
    DLInitList(&F, dispose_inst);

    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), &pomocna[4], get_adress("a",f)));


    symbol_table_t * f2 = create_function_symbol_table();
    insert_function_symbol_table("f2", INT, 2, 3, "di", f2);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));


    insert_function_variable_symbol_table(f2, "par1", INT, 0);
    insert_function_variable_symbol_table(f2, "a", INT, 1);
    insert_function_variable_symbol_table(f2, "b", INT, 2);
    insert_function_variable_symbol_table(f2, "c", INT, 3);
    insert_function_variable_symbol_table(f2, "d", INT, 4);

    i = get_symbol_table_function_item(f2, "par1");
    d_print("%d", i->variable.data_type == INT);

    pomocna[10].i=4;
    pomocna[11].i=7;
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna[10], NULL, get_adress("a",f)));
    DLInsertLast(&L, generate(I_ASSIGN, &pomocna[11], NULL, get_adress("b",f)));
    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), get_adress("b",f), get_adress("a",f)));

    int x = 5;//pocet premennych plus parametrov
    DLInsertLast(&L, generate(I_INIT_FRAME, &x, NULL, NULL));
    DLInsertLast(&L, generate(I_PUSH_PARAM, get_adress("a",f), NULL, NULL));
    DLInsertLast(&L, generate(I_PUSH_PARAM, get_adress("a",f), NULL, NULL));

    pomocna[16].offset = -1;
    pomocna[16].i = 1;
    pomocna[16].data_type = INT;
    DLInsertLast(&L, generate(I_F_CALL, &F, NULL, &pomocna[16]));


    tDLList I;
    DLInitList(&I, dispose_inst);
    DLInsertLast(&I, generate(I_ADD, get_adress("par1",f2), get_adress("par1",f2), get_adress("par1",f2)));
    DLInsertLast(&I, generate(I_ADD, get_adress("par1",f2), get_adress("par1",f2), get_adress("par1",f2)));
    //F
    pomocna[15].offset = -1;
    pomocna[15].i = 1;
    DLInsertLast(&F, generate(I_SUB, get_adress("par1",f2), &pomocna[15], get_adress("a",f2)));
    


    pomocna[13].i = 1;
    pomocna[13].offset = -1;
    pomocna[14].offset = -1;
    //if (par1 > 1000) { volaj f }
    DLInsertLast(&F, generate(I_G, get_adress("a",f2), &pomocna[13], &pomocna[14]));
    DLInsertLast(&F, generate(I_JNT, &pomocna[14], NULL, NULL));
    js_push(DLGetLast(&F));

    DLInsertLast(&F, generate(I_INIT_FRAME, &x, NULL, NULL));
    DLInsertLast(&F, generate(I_PUSH_PARAM, get_adress("a",f2), NULL, NULL));
    DLInsertLast(&F, generate(I_F_CALL, &F, NULL, get_adress("a",f2)));
    set_label(js_top(), DLGetLast(&F));
    js_pop();
 
    DLInsertLast(&F, generate(I_MUL, get_adress("par1",f2), get_adress("a",f2), get_adress("a",f2)));
    DLInsertLast(&F, generate(I_RETURN, get_adress("a",f2), NULL, NULL));





    //DLInsertLast(&F, generate(I_SUB, get_adress("par1",f2), get_adress("par1",f2), get_adress("par1",f2)));





    DLInsertLast(&L, generate(I_ADD, get_adress("a",f), get_adress("b",f), get_adress("a",f)));







    processed_tape = &L;
    interpret_tac(&L);

    DLDisposeList(&L);
    DLDisposeList(&F);
    DLDisposeList(&I);






    free_class_list();

    return 0;
}
