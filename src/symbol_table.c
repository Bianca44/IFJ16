#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"
#include "symbol_table.h"
#include "ial.h"
#include "scanner.h"
#include "DLList.h"

symbol_table_t *class_list;
char* current_class;

/* Zrusi tabulku obsahuju tabulky symbolov tried */
void dispose_class_list(tData data) {
    printf("mazem\n");
        ht_free((symbol_table_t *)(data));
}

/* Zrusi tabulku symbolov pre triedu */
void dispose_class_symbol_table(tData data) {
        symbol_table_item_t * item = (symbol_table_item_t *) data;
        printf("FREE %s\n", item->id_name);
        //free(item->id_name);


        if (item->is_function) {
                if (item->function.param_data_types != NULL) {
                        free(item->function.param_data_types);
                }
                if (item->function.symbol_table != NULL) {
                        printf("nenu null %d\n", item->function.symbol_table->n_items);
                        ht_free((symbol_table_t *)(item->function.symbol_table));
                }
                // pasku
                //ht_free((symbol_table_t *)(item->function.symbol_table));
        } else {
                if (item->variable.s != NULL) {
                        free(item->variable.s);
                }
        }
        free(item);
}

/* Inicializacia zoznamu tried */
void init_class_list() {
        class_list = ht_init(SYMBOL_TABLE_SIZE, hash_code, dispose_class_list); /* TODO */
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
        p->is_function = false;
        p->declared = true;
        ht_insert(symbol_table, id_name, p);
        return p;
}

/* Vlozi polozku - funkciu -  do tabulky symbolov aktualnej triedy */
symbol_table_item_t * insert_function_symbol_table(char * id_name, int data_type, int params_count, int local_vars_count, char * param_data_types, symbol_table_t * symbol_table) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->id_name = id_name;
        p->function.return_type = data_type;
        p->function.params_count = params_count;
        p->function.local_vars_count = local_vars_count;
        p->function.param_data_types = param_data_types;
        p->function.symbol_table = symbol_table;
        p->is_function = true;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return p;
}

/* Vrati polozku z tabulky symbolov pre danu triedu */
symbol_table_item_t * get_symbol_table_class_item(char * class_name, char * id_name) {
        symbol_table_t * sym_table = get_symbol_table_for_class(class_name);
        if (sym_table == NULL) {
                return NULL;
        }
        symbol_table_item_t * sym_table_item = ht_read(sym_table, id_name);
        return sym_table_item;
}

/* Zisti ci existuje premenna alebo funkcia v aktualnej triede */
bool is_declared(char * id_name) {
        symbol_table_item_t * sym_table_item = get_symbol_table_class_item(current_class, id_name);
        if (sym_table_item == NULL) {
                return false;
        }
        return sym_table_item->declared;
}

/* Vrati polozku z tabulky symbolov funkcie */
symbol_table_item_t * get_symbol_table_function_item(symbol_table_t * symbol_table, char * id_name) {
        symbol_table_item_t * sym_table_item = ht_read(symbol_table, id_name);
        if (sym_table_item == NULL) {
                return NULL;
        }

        return sym_table_item;
}

/* Zisti ci je parameter/premenna deklarovana vo funkcii */
bool is_declared_in_function(symbol_table_t * symbol_table, char * id_name) {
        symbol_table_item_t * sym_table_item = get_symbol_table_function_item(symbol_table, id_name);
        if (sym_table_item == NULL) {
                return false;
        }
        return sym_table_item->declared;
}

/* Prevod datoveho typu na jeho specificky znak */
void append_param_data_types(int type) {
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

        append_char(&param_data_types, data_type_char);
}


/* Ziska polozku za tabulky symbolov pre plne kvalifikovany identifikator */
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

/* Uvolni zoznam tried */
void free_class_list() {
        ht_free(class_list);
}

void js_init() {
        head = NULL;
}

void js_push(tDLElemPtr instr) {
        js_item * p = malloc(sizeof(js_item));
        p->data = instr;
        p->next = head;
        head = p;
}

tDLElemPtr js_top() {
        return head->data;
}

void js_pop() {
        js_item * tmp = head;
        head = head->next;
        free(tmp);
}
