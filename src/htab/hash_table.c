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
unsigned hash_code(const tKey key, unsigned ht_size) { 
  unsigned h=0; 
  const unsigned char *p; 
  for(p=(const unsigned char*)key; *p!='\0'; p++)
  {
    h = 65599*h + *p; 
  }
  return h % ht_size; 
}
/**
 * @brief Initializes new hash table
 * @param ht_size size of hash table
 * @return hash table (pointer) or NULL in case of failure
 */
tHTable * ht_init(unsigned ht_size, unsigned int (*hash_code_ptr)(const tKey, unsigned)){

    tHTable *new_table;
    //allocating space for table 
    if((new_table = malloc(sizeof(tHTable) + ht_size * sizeof(void *))) == NULL){
       return NULL; //error
    }

    /*setting values of new hash table*/
    new_table->ht_size = ht_size;
    new_table->hash_code_ptr = hash_code_ptr;
    new_table->n_items = 0;
    
    for(unsigned i=0; i < ht_size; i++){
        new_table->ptr[i] = NULL;
    }

   return new_table;
}

/**
 * @brief Frees and delete all items in table but not table itself
 * @param ptrht table to be freed
 * @param dispose_func function that dispose list of items
 * @pre ptrht is not NULL
 */

void ht_clear_all(tHTable *ptrht, void (* dispose_func_ptr)(void *)){

    void *item;

    for(unsigned i = 0; i < ptrht->ht_size; i++){
        
        item = ptrht->ptr[i];

        if(item == NULL){
            continue;
        }
        //if TODO 
        dispose_func_ptr(item);

        ptrht->ptr[i] = NULL;
        
    }

    ptrht->n_items = 0;
}

void ht_free(tHTable *ptrht,void (* dispose_func_ptr)(void *)){
    
    ht_clear_all(ptrht, dispose_func_ptr);  
    free(ptrht); 
}

void * ht_search(tHTable * ptrht, const tKey key, void *(* item_search_ptr)(void *, tKey key)){
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    //accesing list of synonyms
    void *item = ptrht->ptr[index];
    
    return item_search_ptr(item, key); 
}

void ht_insert(tHTable * ptrht, const tKey key, void * data, void (*item_insert_first_ptr)(void *, void *)){    
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    void * item = ptrht->ptr[index];
    item_insert_first_ptr(&item, data);
}

void * ht_read(tHTable * ptrht, const tKey key, void * (* item_get_value_ptr)(void *),void *(* item_search_ptr)(void *, tKey key)){
    
    void * item;

    if((item = ht_search(ptrht, key, item_search_ptr)) == NULL){
        return NULL;
    }

    return item_get_value_ptr(item); 
}

void * item_get_value(void *item){
    return item;
}

void ht_Delete (tHTable *ptrht, const tKey key, void (*item_delete_ptr)(void *)) {
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    void * item = ptrht->ptr[index];

    item_delete_ptr(item);
}

void item_delete(void *item){

}
//Functions for lists
void * item_search(void * item, tKey key){
    tHTitem * tmp = item;
    printf("sdad\n");

    return tmp;
}


void dispose_ht_item(void * list){
    
    //t_htab_listitem_ptr tmp = list;
    
    return ;
}
void item_insert_first(void * item, void * data){
    return;
}
