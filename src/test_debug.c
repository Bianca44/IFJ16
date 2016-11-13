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
    //set_current_class(name);
    insert_class(name);
    set_current_class(name);

    symbol_table_t * t = get_symbol_table_for_class(name);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));

    symbol_table_t * f = create_function_symbol_table();

    insert_function_variable_symbol_table(f, "par1", DOUBLE, 0);
    insert_function_variable_symbol_table(f, "par2", INT, 1);
    insert_function_variable_symbol_table(f, "a", INT, 1);
    insert_function_variable_symbol_table(f, "b", INT, 1);
    insert_function_variable_symbol_table(f, "c", INT, 1);
    insert_function_variable_symbol_table(f, "d", DOUBLE, 1);
    insert_function_variable_symbol_table(f, "e", DOUBLE, 1);
    insert_function_variable_symbol_table(f, "f", DOUBLE, 1);
    insert_function_variable_symbol_table(f, "bool", BOOLEAN, 1);

    insert_function_symbol_table("test", INT, 2, 3, "di", f);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));

    symbol_table_item_t *i = get_symbol_table_function_item(f, "par1");
    d_print("%d", i->variable.data_type == DOUBLE);


    //insert(f->instrukcn, generate(ADD, adresu1, adresu2, adresu2));

    tDLList L;
    DLInitList(&L, dispose_inst);

    tVar pomocna[100];
    pomocna[0].i = 100;
    pomocna[1].i = 35;
    pomocna[2].d = 2.5;
    pomocna[3].d = 3;

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


    tDLElemPtr inst;

    DLInsertLast(&L, generate(I_G, get_adress("a",f), get_adress("b",f), get_adress("bool",f)));
    //uloz na zosobnik
    DLInsertLast(&L, generate(I_JNT, get_adress("bool",f), &L, NULL));
    //Push(DLGetLast(&L));
    DLInsertLast(&L, generate(I_MUL, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
    DLInsertLast(&L, generate(I_GOTO, NULL, NULL, NULL));
    //set_label(top, DLGetLast(&L));
    //pop
    //push(DLGetLast);
    DLInsertLast(&L, generate(I_DIV, get_adress("a",f), get_adress("b",f), get_adress("c",f)));
    //set_label(top, DLGetLast(&L));
    //pop
    //

    //set_label(tDLElemPtr jump, tDLElemPtr label)
    //{
    //  ((tVar *)(jump->data))->result = label;
    //}



    js_init();
    inst = DLGetLast(&L);
    js_push(inst);
    tDLElemPtr a = js_top();
    printf("MEM: %p vs %p\n", a, inst);

    DLInsertLast(&L, generate(I_ADD, get_adress("d",f), get_adress("e",f), get_adress("f",f)));
    DLInsertLast(&L, generate(I_SUB, get_adress("d",f), get_adress("e",f), get_adress("f",f)));

    interpret_tac(&L);

    DLDisposeList(&L);
    free_class_list();

    return 0;
}
