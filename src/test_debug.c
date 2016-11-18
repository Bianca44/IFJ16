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
#include "memory_constants.h"

int push_counter;
tDLList * processed_tape;
tVar * global_offset;

tFrameStack frame_stack;
symbol_table_t *class_list;
symbol_table_item_t current_variable;
symbol_table_item_t function_variable;
symbol_table_item_t current_function;
string_t param_data_types;
constant_t *labels;


tVar * get_adress(char *id, symbol_table_t *t){
   
    return &((symbol_table_item_t *)ht_read(t, id))->variable;
}

void set_label(tDLElemPtr jump, tDLElemPtr where){
    //malloc a pridat offset v kombinacii s povedomim o vykonavanej paske
    //linearny zoznam
    //TODO

    ((tInst *)(jump->data))->result = insert_special_const(&labels, (void *)where);

}

int main(){
    
    
    
    //pomocne premenna na simulovanie konstant
    tVar pomocna[100];
    for(int i=0 ;i < 100; i++){

        pomocna[i].offset = -1;
        
    }

    //filling symbol table for test purposes
    init_class_list();

    //class Main
    //{
    //  static void run()
    //  {
    //      //INT
    //      a = b + c;
    //      a = b - c;
    //      a = b * c;
    //      a = b / c;
    //      //FLOAT
    //      d = e + f;
    //      d = e - f;
    //      d = e * f;
    //      d = e / f;
    //  }
    //}

    char * name = "Main";
    //pridanie triedy
    insert_class(name);
    //nastavime ako aktulanu triedu
    set_current_class(name);

    //pristup k tabulke
    symbol_table_t * t = get_symbol_table_for_class(name);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));

    //vytvorenie tabulka symbolov pre funkciu
    symbol_table_t * run = create_function_symbol_table();

    //pridanie lokalnych premennych
    insert_function_variable_symbol_table(run, "a", INT, 0);
    insert_function_variable_symbol_table(run, "b", INT, 1);
    insert_function_variable_symbol_table(run, "c", INT, 2);
    insert_function_variable_symbol_table(run, "d", DOUBLE, 3);
    insert_function_variable_symbol_table(run, "e", DOUBLE, 4);
    insert_function_variable_symbol_table(run, "f", DOUBLE, 5);
    insert_function_variable_symbol_table(run, "bool", BOOLEAN, 6);

    //pridanie ts funkcie do aktualnej triedy
    insert_function_symbol_table("run", VOID, 0, 7, "", run);
    d_print("pocet funkcii: %d existuje Main: %d", t->n_items, exists_class("Main"));

    //global instruction tape
    tDLList gl_tape;
    DLInitList(&gl_tape, dispose_inst);
      
    pomocna[0].i = 30; //a
    pomocna[1].i = 35; //b
    pomocna[2].d = 10.2; //e
    pomocna[3].d = 2.57; //f

    int x = 10;  
    //paska pre run  
    tDLList run_tape;
    DLInitList(&run_tape, dispose_inst);

    DLInsertLast(&gl_tape, generate(I_INIT_FRAME, &x, NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_F_CALL, &run_tape, NULL, NULL));

    //b = 30
    DLInsertLast(&run_tape, generate(I_ASSIGN, &pomocna[0], NULL, get_adress("b",run)));
    //c = 35
    DLInsertLast(&run_tape, generate(I_ASSIGN, &pomocna[1], NULL, get_adress("c",run)));
    //      a = b + c;
    //      a = b - c;
    //      a = b * c;
    //      a = b / c;
    DLInsertLast(&run_tape, generate(I_ADD, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_SUB, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_MUL, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_DIV, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    //d = 10.2
    DLInsertLast(&run_tape, generate(I_ASSIGN, &pomocna[2], NULL, get_adress("e",run)));
    //e = 2.57
    DLInsertLast(&run_tape, generate(I_ASSIGN, &pomocna[3], NULL, get_adress("f",run)));
    //      d = e + f;
    //      d = e - f;
    //      d = e * f;
    //      d = e / f;
    DLInsertLast(&run_tape, generate(I_ADD, get_adress("e",run), get_adress("f",run), get_adress("d",run)));
    DLInsertLast(&run_tape, generate(I_SUB, get_adress("e",run), get_adress("f",run), get_adress("d",run)));
    DLInsertLast(&run_tape, generate(I_MUL, get_adress("e",run), get_adress("f",run), get_adress("d",run)));
    DLInsertLast(&run_tape, generate(I_DIV, get_adress("e",run), get_adress("f",run), get_adress("d",run)));
    DLInsertLast(&run_tape, generate(I_RETURN, NULL, NULL, NULL));
/*
    
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



    DLDisposeList(&F);
    DLDisposeList(&I);

    */
    //interpret_tac(&gl_tape);
    DLDisposeList(&gl_tape);
    DLDisposeList(&run_tape);
    free_class_list();
    free_constants(&labels);
    free_constants(&tape_ref);

    return 0;
}
