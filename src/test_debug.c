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


symbol_table_t *class_list;
symbol_table_item_t current_variable;
symbol_table_item_t function_variable;
symbol_table_item_t current_function;
string_t param_data_types;

tVar * get_adress(char *id, symbol_table_t *t){
   
    return &((symbol_table_item_t *)ht_read(t, id))->variable;
}



int main(){
    
    
    
    //pomocne premenna na simulovanie konstant
    tVar pomocna[100];
    for(int i=0 ;i < 100; i++){

        pomocna[i].offset = CONSTANT;
        pomocna[i].initialized = true;
        
    }

    //filling symbol table for test purposes
    init_class_list();

    //class Main
    //{
    //  static int x;
    //  static void run()
    //  {
    //      //INT
    //      a = b + c;
    //      a = b - c;
    //      a = b * c;
    //      a = b / c;
    //      x = x + b;
    //      //FLOAT
    //      d = e + f;
    //      d = e - f;
    //      d = e * f;
    //      d = e / f;
    //      
    //      if(b < c)
    //      {
    //          a = b*c
    //      }
    //      else
    //      {
    //          a = b/c
    //      }
    //      
    //      while(b <= c){
    //          b = b + 1;
    //      }
    //  }
    //  x = x + x; // **
    //
    //  static int factorial(int n) // Definice funkce pro vypocet faktorialu
    //  {
    //      int temp_result;
    //      int decremented_n = n - 1;
    //          if (n < 2) {
    //              return 1;
    //          }
    //          else {
    //              temp_result = factorial(decremented_n);
    //              temp_result = n * temp_result;
    //              return temp_result;
    //      }
    //  }
    //  y = readInt();
    //  x = factorial(y);
    //}

    char * name = "Main";
    //pridanie triedy
    insert_class(name);
    //nastavime ako aktulanu triedu
    set_current_class(name);

    //pristup k tabulke
    symbol_table_t * main_f = get_symbol_table_for_class(name);
    d_print("pocet funkcii: %d existuje Main: %d", main_f->n_items, exists_class("Main"));
    //x
    insert_variable_symbol_table("x", INT, CONSTANT);
    insert_variable_symbol_table("y", INT, CONSTANT);
    insert_variable_symbol_table("str", STRING, CONSTANT);

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
    insert_function_symbol_table("run", VOID, 0, 7, "","iiidddb", run);
    d_print("pocet funkcii: %d existuje Main: %d", main_f->n_items, exists_class("Main"));

    //global instruction tape
    tDLList gl_tape;
    DLInitList(&gl_tape, dispose_inst);
      
    pomocna[0].i = 32; //b
    pomocna[0].data_type = INT; //b
    pomocna[1].i = 35; //c
    pomocna[1].data_type = INT; //c
    pomocna[2].d = 10.2; //e
    pomocna[2].data_type = DOUBLE; //e
    pomocna[3].d = 2.57; //f
    pomocna[3].data_type = DOUBLE; //f
    pomocna[4].i = 2;
    pomocna[4].data_type = INT;
    symbol_table_item_t *x = get_symbol_table_class_item("Main", "run");

    x->function.params_local_vars_count = 7;
    //paska pre run  
    tDLList  run_tape;
    DLInitList(&run_tape, dispose_inst);
    // x = 2
    DLInsertLast(&gl_tape, generate(I_ASSIGN, &pomocna[4], NULL, get_adress("x",main_f)));
    // run()
    DLInsertLast(&gl_tape, generate(I_INIT_FRAME, x, NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_F_CALL, &run_tape, NULL, NULL));

    //ZACIATOK FUNKCIE ========================
    //b = 30
    DLInsertLast(&run_tape, generate(I_ASSIGN, &pomocna[0], NULL, get_adress("b",run)));
    //c = 35
    DLInsertLast(&run_tape, generate(I_ASSIGN, &pomocna[1], NULL, get_adress("c",run)));
    //      a = b + c;
    //      a = b - c;
    //      a = b * c;
    //      a = b / c;
    //      x = x + b;
    DLInsertLast(&run_tape, generate(I_ADD, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_SUB, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_MUL, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_DIV, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    DLInsertLast(&run_tape, generate(I_ADD, get_adress("x",main_f), get_adress("b",run), get_adress("x",main_f)));
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



    //x = x + x
    DLInsertLast(&gl_tape, generate(I_ADD, get_adress("x",main_f), get_adress("x",main_f), get_adress("x",main_f)));
    //print(x) 
    DLInsertLast(&gl_tape, generate(I_PRINT, get_adress("x",main_f), NULL, NULL));
    //if(b < c)
    //{
    //  a = b*c
    //}
    //else
    //{
    //  a = b/c
    //}

    //ZACIATOK PODM ===================
    js_init(); // zasobnik na to odkial sa skace alebo kde skocit
    
    // if a > b
    DLInsertLast(&run_tape, generate(I_L, get_adress("b",run), get_adress("c",run), get_adress("bool",run)));
    DLInsertLast(&run_tape, generate(I_JNT, get_adress("bool",run), NULL, NULL));
    js_push(DLGetLast(&run_tape)); //uloz na zosobnik, odkial sa skace
    //c = a * b
    DLInsertLast(&run_tape, generate(I_MUL, get_adress("b",run), get_adress("c",run), get_adress("a",run)));

    DLInsertLast(&run_tape, generate(I_GOTO, NULL, NULL, NULL));
  
    set_label(js_top(), DLGetLast(&run_tape));//na if nastavime ze ma skocit za goto

    js_pop();//uvolnime polozku odkial sa skakalo (z ifu)
    js_push(DLGetLast(&run_tape)); // pridame odkial sa skace (z goto)
    //c = a/b    
    DLInsertLast(&run_tape, generate(I_DIV, get_adress("b",run), get_adress("c",run), get_adress("a",run)));
    set_label(js_top(), DLGetLast(&run_tape)); //do goto priradime adresu de ma skocit (za DIV)
    js_pop();

   //KONIEC PODM =======================

    //while(b <= c){
    //  b = b + 1;
    //  }
    pomocna[5].i = 1;
    pomocna[5].data_type = INT;
    //  ZACIATOK CYKLU
    js_push(DLGetLast(&run_tape)); //pridanie de sa ma skocit, - bude sa pokraocvat na overeni podm
    // b <= c
    DLInsertLast(&run_tape, generate(I_LE, get_adress("b",run), get_adress("c",run), get_adress("bool",run)));
    DLInsertLast(&run_tape, generate(I_JNT, get_adress("bool",run), NULL, NULL));
    js_push(DLGetLast(&run_tape)); //odkial sa skace
    //b = b + 1
    DLInsertLast(&run_tape, generate(I_ADD, get_adress("b",run), &pomocna[5], get_adress("b",run)));
    
    DLInsertLast(&run_tape, generate(I_GOTO, NULL, NULL, NULL));
    set_label(js_top(), DLGetLast(&run_tape));//do ifu nastavime ze sa ma skocit za goto
    js_pop();
    set_label(DLGetLast(&run_tape), js_top()); // do goto priradime ze sa ma skocit pred while na overenie podm
    js_pop();
    //KONIEC CYKLU ==============

    DLInsertLast(&run_tape, generate(I_RETURN, NULL, NULL, NULL));

    //KONIEC FUNKCIE ============================================



    //ZACIATOK FAKTORIALU  ========================================
    //static int factorial(int n) // Definice funkce pro vypocet faktorialu
    //{
    //  int temp_result;
    //  int decremented_n = n - 1;
    //      if (n < 2) {
    //          return 1;
    //      }
    //      else {
    //          temp_result = factorial(decremented_n);
    //          temp_result = n * temp_result;
    //          return temp_result;
    //      }
    //}

    //vytvorenie tabulka symbolov pre funkciu
    symbol_table_t * fact = create_function_symbol_table();

    //pridanie lokalnych premennych
    insert_function_variable_symbol_table(fact, "n", INT, 0);
    insert_function_variable_symbol_table(fact, "temp_result", INT, 1);
    insert_function_variable_symbol_table(fact, "decremented_n", INT, 2);
    insert_function_variable_symbol_table(fact, "bool", BOOLEAN, 3);

    //pridanie ts funkcie do aktualnej triedy
    insert_function_symbol_table("fact", INT, 1, 4, "i","iib", fact);
    d_print("pocet funkcii: %d existuje Main: %d", main_f->n_items, exists_class("Main"));

    //paska pre fact 
    tDLList  fact_tape;
    DLInitList(&fact_tape, dispose_inst);
    symbol_table_item_t *z = get_symbol_table_class_item("Main", "fact");
    z->function.params_local_vars_count = 4;
    d_int(z->function.params_local_vars_count);
    pomocna[6].i = 11;
    pomocna[6].data_type = INT;
    pomocna[9].s = "Zadajte cislo faktorial n:\n";
    pomocna[9].data_type = STRING;
    //y = readInt();

    DLInsertLast(&gl_tape, generate(I_PRINT, &pomocna[9], NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_RINT, NULL, NULL, get_adress("y",main_f)));

    // x = factorial(y)
    DLInsertLast(&gl_tape, generate(I_INIT_FRAME, z, NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_PUSH_PARAM, get_adress("y",main_f), NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_F_CALL, &fact_tape, NULL, get_adress("x",main_f)));


    pomocna[7].i = 1;
    pomocna[7].data_type = INT;
    DLInsertLast(&fact_tape, generate(I_SUB, get_adress("n", fact), &pomocna[7], get_adress("decremented_n",fact)));
    
    pomocna[8].i = 2;
    pomocna[8].data_type = INT;

    // if n < 2
    DLInsertLast(&fact_tape, generate(I_L, get_adress("n",fact), &pomocna[8], get_adress("bool",fact)));
    DLInsertLast(&fact_tape, generate(I_JNT, get_adress("bool",fact), NULL, NULL));
    js_push(DLGetLast(&fact_tape)); //uloz na zosobnik, odkial sa skace
    //return 1
    DLInsertLast(&fact_tape, generate(I_RETURN, &pomocna[7], NULL, NULL));
    DLInsertLast(&fact_tape, generate(I_GOTO, NULL, NULL, NULL));  
    set_label(js_top(), DLGetLast(&fact_tape));//na if nastavime ze ma skocit za goto
    js_pop();//uvolnime polozku odkial sa skakalo (z ifu)
    js_push(DLGetLast(&fact_tape)); // pridame odkial sa skace (z goto)
    //temp_result = factorial(decremented_n)
    DLInsertLast(&fact_tape, generate(I_INIT_FRAME, z, NULL, NULL));
    DLInsertLast(&fact_tape, generate(I_PUSH_PARAM, get_adress("decremented_n",fact), NULL, NULL));
    DLInsertLast(&fact_tape, generate(I_F_CALL, &fact_tape, NULL, get_adress("temp_result",fact)));
    //temp_result = n * temp_result
    DLInsertLast(&fact_tape, generate(I_MUL, get_adress("n", fact), get_adress("temp_result",fact), get_adress("temp_result",fact)));
    //return temp_result
    DLInsertLast(&fact_tape, generate(I_RETURN, get_adress("temp_result", fact), NULL, NULL));

    set_label(js_top(), DLGetLast(&fact_tape)); //do goto priradime adresu de ma skocit (za DIV)
    js_pop(); 
    //KONIEC FAKTORIALU  ========================================
    //y = readInt();
    //str = readString();
    DLInsertLast(&gl_tape, generate(I_RINT, NULL, NULL, get_adress("y",main_f)));
    DLInsertLast(&gl_tape, generate(I_RSTR, NULL, NULL, get_adress("str",main_f)));
    //print(y)
    //print str
    //sort(str)
    //len(str)
    //print(len)
    pomocna[10].data_type = INT;
    DLInsertLast(&gl_tape, generate(I_PRINT, get_adress("y",main_f), NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_PRINT, get_adress("str",main_f), NULL, NULL));
   

    //sort 
    symbol_table_t * substr = create_function_symbol_table();
    insert_function_symbol_table("substr", STRING, 3, 0, "sii","", substr);
    symbol_table_item_t *y = get_symbol_table_class_item("Main", "substr");
    y->function.params_local_vars_count = 3; 
    pomocna[11].data_type = INT;
    pomocna[11].i = 1;
    pomocna[12].data_type = INT;
    pomocna[12].i = 3;
    DLInsertLast(&gl_tape, generate(I_INIT_FRAME, y, NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_PUSH_PARAM, get_adress("str",main_f), NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_PUSH_PARAM, &pomocna[11], NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_PUSH_PARAM, &pomocna[12], NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_SUBSTR, NULL, NULL, get_adress("str",main_f)));

    DLInsertLast(&gl_tape, generate(I_SORT, get_adress("str",main_f), NULL, get_adress("str",main_f)));
    DLInsertLast(&gl_tape, generate(I_PRINT, get_adress("str",main_f), NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_LEN, get_adress("str",main_f), NULL, &pomocna[10]));
    DLInsertLast(&gl_tape, generate(I_PRINT, &pomocna[10], NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_TO_STRING, &pomocna[10], NULL, get_adress("str",main_f)));
    DLInsertLast(&gl_tape, generate(I_PRINT, get_adress("str",main_f), NULL, NULL));
    DLInsertLast(&gl_tape, generate(I_CAT, get_adress("str",main_f), get_adress("str",main_f), get_adress("str",main_f)));
    DLInsertLast(&gl_tape, generate(I_CAT, get_adress("str",main_f), get_adress("str",main_f), get_adress("str",main_f)));
    DLInsertLast(&gl_tape, generate(I_CAT, get_adress("str",main_f), get_adress("str",main_f), get_adress("str",main_f)));
    DLInsertLast(&gl_tape, generate(I_CAT, get_adress("str",main_f), get_adress("str",main_f), get_adress("str",main_f)));
    DLInsertLast(&gl_tape, generate(I_PRINT, get_adress("str",main_f), NULL, NULL));

    processed_tape = &gl_tape;
    interpret_tac(&gl_tape);
    DLDisposeList(&gl_tape);
    DLDisposeList(&run_tape);
    DLDisposeList(&fact_tape);
    free_class_list();
    free_constants(&labels);
    free_constants(&tape_ref);



    return 0;
}
