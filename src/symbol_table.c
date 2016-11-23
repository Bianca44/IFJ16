#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"
#include "symbol_table.h"
#include "ial.h"
#include "scanner.h"

symbol_table_t *class_list;
char* current_class;

/* Zrusi tabulku obsahuju tabulky symbolov tried */
void dispose_class_list(tData data) {
        ht_free((symbol_table_t *)(data));
}

/* Zrusi tabulku symbolov pre triedu */
void dispose_class_symbol_table(tData data) {
        symbol_table_item_t * item = (symbol_table_item_t *) data;
        //free(item->id_name);


        if (item->is_function) {
                if (item->function.param_data_types != NULL) {
//                        free(item->function.param_data_types);
                }
                if (item->function.local_vars_data_types != NULL) {
  //                      free(item->function.local_vars_data_types);
                }
                if (item->function.symbol_table != NULL) {
                        ht_free((symbol_table_t *)(item->function.symbol_table));
                }

                if (item->function.instruction_tape != NULL) {
      //                  DisposeList(item->function.instruction_tape);
                }
        } else {
                if(item->variable.initialized && item->variable.data_type == STRING){
                        free(item->variable.s);
                }
        }
        free(item);
}

/* Inicializacia zoznamu tried */
void init_class_list() {
        class_list = ht_init(CLASS_TABLE_SIZE, hash_code, dispose_class_list);
}

/* Nastavi aktualnu triedu */
void set_current_class(char *class_name) {
        current_class = class_name;
}

/* Vlozi triedu do zoznamu tried */
bool insert_class(char* class_name) {
        symbol_table_t *symbol_table = ht_init(SYMBOL_TABLE_SIZE, hash_code, dispose_class_symbol_table);
        ht_insert(class_list, class_name, symbol_table);
        return true;
}

/* Vytvori (lokalnu) tabulku symbolov pre funkciu */
symbol_table_t * create_function_symbol_table() {
        symbol_table_t * p = ht_init(SYMBOL_TABLE_SIZE, hash_code, dispose_class_symbol_table);
        if (p == NULL) return NULL;
        return p;
}

/* Vrati tabulku symbolov pre danu triedu */
symbol_table_t * get_symbol_table_for_class(char* class_name) {
        if (class_list == NULL) {
                return NULL;
        }
        symbol_table_t * p = ht_read(class_list, class_name);
        if (p == NULL) return NULL;
        return p;
}

/* Najde funkciu v triede a vrati tabulku symbolov pre danu funkciu */
symbol_table_t * get_symbol_table_for_function(char *class_name, char * id_name) {
        symbol_table_item_t * item = get_symbol_table_class_item(class_name, id_name);
        if (item == NULL) {
                return NULL;
        }
        return item->function.symbol_table;
}

/* Vlozi polozku tabulky symbolov do tabulky symbolov pre triedu */
bool insert_symbol_table_item_class(char * class_name, char * id_name, void * data) {
        symbol_table_t * symbol_table = get_symbol_table_for_class(class_name);
        ht_insert(symbol_table, id_name, data);
        return true;

}

/* Vlozi polozku tabulky symbolov do tabulky symbolov pre aktualnu triedu v ktorej sme vo vnutri */
bool insert_symbol_table_item(char * id_name, void * data) {
        return insert_symbol_table_item_class(current_class, id_name, data);

}

/* Zistenie ci trieda existuje */
bool exists_class(char* class_name) {
        return get_symbol_table_for_class(class_name) != NULL;
}

/* Vytvori polozku tabulky symbolov */
symbol_table_item_t * create_symbol_table_item() {
        symbol_table_item_t * p = (symbol_table_item_t *) malloc(sizeof(struct symbol_table_item));
        return p;
}

/* Vlozi polozku - globalnu (staticku) premennu - do tabulky symbolov aktualnej triedy */
symbol_table_item_t * insert_variable_symbol_table(char * id_name, int data_type, int offset) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->variable.data_type = data_type;
        p->variable.offset = offset;
        p->variable.initialized = false;
        if (data_type == STRING) {
            p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return p;
}

/* Vlozi parametre funkcie/lokalne premenne funkcie do tabulky symbolov pre danu funkciu */
symbol_table_item_t * insert_function_variable_symbol_table(symbol_table_t *symbol_table, char * id_name, int data_type, int offset) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->id_name = id_name;
        p->variable.data_type = data_type;
        p->variable.offset = offset;
        p->variable.initialized = false;
        if (data_type == STRING) {
            p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;
        ht_insert(symbol_table, id_name, p);
        return p;
}

/* Vlozi polozku - funkciu -  do tabulky symbolov aktualnej triedy */
symbol_table_item_t * insert_function_symbol_table(char * id_name, int data_type, int params_count, int local_vars_count, char * param_data_types, char * local_vars_data_types, symbol_table_t * symbol_table) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->id_name = id_name;
        p->function.return_type = data_type;
        p->function.params_count = params_count;
        p->function.local_vars_count = local_vars_count;
        p->function.params_local_vars_count = params_count + local_vars_count;
        p->function.param_data_types = param_data_types;
        if (local_vars_data_types == NULL) {
            local_vars_data_types = copy_string("");
        }
        p->function.local_vars_data_types = local_vars_data_types;
        p->function.symbol_table = symbol_table;
        p->is_function = true;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return p;
}

void insert_instr_tape_for_function(char * class_name, char * function_name, tList * tape) {
    symbol_table_item_t * function_item = get_symbol_table_class_item(class_name, function_name);
    function_item->function.instruction_tape = tape;
}

/* Vrati polozku z tabulky symbolov pre danu triedu */
symbol_table_item_t * get_symbol_table_class_item(char * class_name, char * id_name) {
        if (id_name == NULL) {
                return NULL;
        }
        symbol_table_t * symbol_table = get_symbol_table_for_class(class_name);
        if (symbol_table == NULL) {
                return NULL;
        }
        symbol_table_item_t * symbol_table_item = ht_read(symbol_table, id_name);
        return symbol_table_item;
}

/* Zisti ci existuje premenna alebo funkcia v aktualnej triede */
bool is_declared(char * id_name) {
        symbol_table_item_t * symbol_table_item = get_symbol_table_class_item(current_class, id_name);
        if (symbol_table_item == NULL) {
                return false;
        }
        return symbol_table_item->declared;
}

/* Vrati polozku z tabulky symbolov funkcie */
symbol_table_item_t * get_symbol_table_function_item(symbol_table_t * symbol_table, char * id_name) {
        symbol_table_item_t * symbol_table_item = ht_read(symbol_table, id_name);
        if (symbol_table_item == NULL) {
                return NULL;
        }

        return symbol_table_item;
}

/* Zisti ci je parameter/premenna deklarovana vo funkcii */
bool is_declared_in_function(symbol_table_t * symbol_table, char * id_name) {
        symbol_table_item_t * symbol_table_item = get_symbol_table_function_item(symbol_table, id_name);
        if (symbol_table_item == NULL) {
                return false;
        }
        return symbol_table_item->declared;
}

/* Prevod datoveho typu na jeho specificky znak */
void append_type(string_t *str, int type) {
        char data_type_char = 0;
        switch (type) {
        case INT:
                data_type_char = 'i';
                break;
        case DOUBLE:
                data_type_char = 'd';
                break;
        case STRING:
                data_type_char = 's';
                break;
        case BOOLEAN:
                data_type_char = 'b';
                break;
        default:
                break;
        }

        append_char(str, data_type_char);
}


/* Ziska polozku z tabulky symbolov pre plne kvalifikovany identifikator */
symbol_table_item_t * get_symbol_table_special_id_item(char * id_name) {
        char *special_id = copy_string(id_name);
        char *class;
        char *method;
        char *delimeter = ".";
        class = strtok(special_id, delimeter);
        method = strtok(NULL, delimeter);

        symbol_table_item_t * item = get_symbol_table_class_item(class, method);
        free(special_id);
        if (item == NULL) {
                return NULL;
        }
        return item;
}

/*
 * Zisti ci existuje dany plne kvalifikovany indentifikator
 * Zisti ci v danej triede existuje dana premenna alebo funkcia
 */
bool is_special_id_declared(char * id_name) {
        symbol_table_item_t * item = get_symbol_table_special_id_item(id_name);
        if (item == NULL) {
                return false;
        }
        return item->declared;
}

/* Vytvori pomocnu premennu v tabulke pre aktualnu triedu */
symbol_table_item_t * insert_tmp_variable_symbol_table_class(int data_type) {
        symbol_table_item_t * p = create_symbol_table_item();
        char *id_name = (char *) malloc(TMP_VAR_NAME_SIZE * sizeof(char));
        static int tmp_id = 0;
        sprintf(id_name, "#%d", tmp_id);

        p->variable.data_type = data_type;
        p->variable.initialized = true;
        if (data_type == STRING) {
            p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;
        p->id_name = id_name;
        p->variable.offset = CONSTANT;
        insert_symbol_table_item(id_name, p);
        tmp_id++;
        return p;
}

/* Vytvori pomocnu premennu v tabulke pre funkciu */
symbol_table_item_t * insert_tmp_variable_symbol_table_function(char * function_name, int data_type) {
        symbol_table_item_t * p = create_symbol_table_item();
        symbol_table_item_t * function_item = get_symbol_table_class_item(current_class, function_name);
        symbol_table_t * function_table = function_item->function.symbol_table;

        int offset = function_item->function.params_local_vars_count;
        char * local_vars_data_types = function_item->function.local_vars_data_types;
        int new_len = strlen(local_vars_data_types) + 1;
        local_vars_data_types = (char *) realloc(local_vars_data_types, (new_len + 1)* sizeof(char));

        int c = 0;
        switch (data_type) {
            case STRING:
                        c = 's';
                        break;
            case INT:
                        c = 'i';
                        break;
            case DOUBLE:
                        c = 's';
                        break;
            case BOOLEAN:
                        c = 'b';
                        break;
        }

        local_vars_data_types[new_len-1] = c;
        local_vars_data_types[new_len] = '\0';
        function_item->function.local_vars_data_types = local_vars_data_types;

        char *id_name = (char *) malloc(TMP_VAR_NAME_SIZE * sizeof(char));
        static int tmp_id = 0;
        sprintf(id_name, "#%d", tmp_id);

        p->id_name = id_name;
        p->variable.data_type = data_type;
        p->variable.offset = offset;
        p->variable.initialized = true;
        if (data_type == STRING) {
            p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;

        function_item->function.params_local_vars_count++;
        function_item->function.local_vars_count++;
        ht_insert(function_table, id_name, p);
        return p;
}

/* Naalokuje instrukcnu pasku pre funkciu */
tList * create_function_instr_tape() {
    tList * tape = (tList *) malloc(sizeof(tList));
    return tape;
}

/* Uvolni zoznam tried */
void free_class_list() {
        ht_free(class_list);
}

void js_init() {
        head = NULL;
}

void js_push(tElemPtr instr) {
        js_item * p = malloc(sizeof(js_item));
        p->data = instr;
        p->next = head;
        head = p;
}

tElemPtr js_top() {
        return head->data;
}

void js_pop() {
        js_item * tmp = head;
        head = head->next;
        free(tmp);
}
