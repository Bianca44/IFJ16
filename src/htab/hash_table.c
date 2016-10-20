#include <stdlib.h>
#include <stdio.h>
#include "hash_table.h"

/**TODO
 * Hash funcion, for each string function generates index 
 * this index is than used for storing/accessing item in hash table
 * @param str word
 * @param htab_size size of hash table
 * @return index according to which we can store/access word in table 
 */

unsigned int hash_function(const char *str, unsigned htab_size) { 
  unsigned int h=0; 
  const unsigned char *p; 
  for(p=(const unsigned char*)str; *p!='\0'; p++)
  {
    h = 65599*h + *p; 
  }
  return h % htab_size; 
}


/**
 * @brief Initializes new hash table
 * @param size size of hash table
 * @param size_of_item size needed for allocating one item
 * @return hash table (pointer) or NULL in case of failure
 */

t_hash_table * hash_table_init(unsigned size){
    t_hash_table *new_table;

    if((new_table = malloc(sizeof(t_hash_table) + size * sizeof(void *))) == NULL){
       return NULL; //error
    }

    /*setting values of new hash table*/
    new_table->htab_size = size;
    new_table->hash_fun_ptr = &hash_function;
    new_table->n_items = 0;
    
    for(unsigned i=0; i<size; i++){
        new_table->ptr[i] = NULL;
    }
   /* 
    struct htab_listitem *item;
    item = malloc(sizeof(struct htab_listitem));
    item->data = 42;
    new_table->ptr[0] = item;
   */      
    return new_table;
}

/**
 * @brief Frees and delete whole table
 * @param t table to be freed
 * @param dispose_func function that dispose list of items
 * @pre t is not NULL
 */

void hash_table_free(t_hash_table *t, void (* dispose_func)(void *)){

    void *item;

    for(unsigned i = 0; i < t->htab_size; i++){
        
        item = t->ptr[i];

        if(item == NULL){
            continue;
        }
        
        dispose_func(item);

        //t->ptr[i] = NULL;
        
    }
    //t->n = 0;
    free(t); 
}


void * hash_table_search_add(t_hash_table *t, void * (*list_find_add)(void *), const char *key){
        
    unsigned index = t->hash_fun_ptr(key, t->htab_size);
    void * tmp = t->ptr[index];    
    printf("in func %d\n", index);

    return list_find_add(tmp);
    
}

void * find_add_listitem(void * list){
    t_htab_listitem_ptr tmp = list;
    printf("sdad\n");

    return tmp;
}


void dispose_htab_listitem(void * list){
    
    //t_htab_listitem_ptr tmp = list;
    
    return ;
}
