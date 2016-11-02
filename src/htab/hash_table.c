#include <stdlib.h>
#include <string.h> //TODO
#include <stdio.h>
#include "hash_table.h"

/**TODO
 * Hash funcion
 * @param key key
 * @param ht_size size of hash table
 * @return index to table
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
 * @param hash_code_ptr hash function
 * @param dispose_func_ptr dispose function for data
 * @return hash table (pointer) or NULL in case of failure
 */
tHTable * ht_init(unsigned ht_size, unsigned (*hash_code_ptr)(const tKey, unsigned),
                  void (*dispose_func_ptr)(tData data)){

    tHTable *new_table;
    //allocating space for table 
    if((new_table = malloc(sizeof(tHTable) + ht_size * sizeof(tSTitem *))) == NULL){
       return NULL; //error
    }

    /*setting values of new hash table*/
    new_table->ht_size = ht_size;
    new_table->hash_code_ptr = hash_code_ptr;
    new_table->dispose_func_ptr = dispose_func_ptr;
    new_table->n_items = 0;
    
    for(unsigned i=0; i < ht_size; i++){
        new_table->ptr[i] = NULL;
    }

   return new_table;
}

/*
 * @brief searches for item in table 
 * @param ptrht hash table
 * @param key key
 * @pre ptrht != NULL and key == NULLL
 * @return item/NULL
 */

tSTitem * ht_search(tHTable * ptrht, const tKey key){
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    //accesing list of synonyms
    tSTitem *first = ptrht->ptr[index];
    
    while(first != NULL){
        //TODO strcmp
        if(strcmp(first->key, key) == 0){
            return first;
        }
 
        first = first->next;
    }
    // not found 
    return NULL;
}

/*
 * @brief inserts item to table with given key, if item exists data are updated
 * @param ptrht hash table
 * @param key key
 * @param data data to be added
 */

void ht_insert(tHTable * ptrht, const tKey key, tData data){    

    tSTitem * item = ht_search(ptrht, key);
    if(item == NULL){
        //getting index
        unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
        //accesing list of synonyms
        tSTitem *first = ptrht->ptr[index];
 
        //adding item
        tSTitem * new;

        if((new = malloc(sizeof(tSTitem))) == NULL){
            return ; //error            
        }

        if((new->key = malloc(sizeof(char) * (strlen(key)+1))) == NULL){
            free(new);
            return ; //error
        }

        //TODO str
        strcpy(new->key, key);
        new->data = data;
        //adding to beginning
        new->next = first;
        ptrht->ptr[index] = new;
        //TODO
        ptrht->n_items += 1;
    }
    else{
        //actualizing item
        //TODO dealocate old data ?
        item->data = data;
    }    

}

/*
 * @brief reads data of item with given key
 * @param ptrht hash table
 * @param key key
 * @return data of given item or NULL if item doesnt exist
 */

tData ht_read(tHTable * ptrht, const tKey key){
    
    tSTitem * item;

    if((item = ht_search(ptrht, key)) == NULL){
        return NULL;
    }

    return item->data;
}

/*
 * @brief deletes item with given key, if item isnt in table nothing happens
 * @param ptrht hash table
 * @param key key
 */
void ht_delete (tHTable * ptrht, tKey key){ 
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    //accesing list of synonyms
    tSTitem * tmp = ptrht->ptr[index];
    tSTitem * tmp_prev = tmp;
     
    while(tmp != NULL){
        //TODO strcmp
        if(strcmp(tmp->key, key) == 0){
            //item to be deleted is first
            if(tmp == tmp_prev){
                ptrht->ptr[index] = tmp->next;
            }
            else{
                tmp_prev->next = tmp->next;
            }
            //freeing item
            free(tmp->key);
            ptrht->dispose_func_ptr(tmp->data);
            free(tmp);
            ptrht->n_items -= 1;
            return; 
        }
        tmp_prev = tmp;
        tmp = tmp->next;
    }

    return;
}


/**
 * @brief Frees and delete all items in table but not table itself
 * @param ptrht table to be freed
 * @param dispose_func function that dispose list of items
 * @pre ptrht is not NULL
 */

void ht_clear_all(tHTable *ptrht){

    tSTitem *first, *tmp;
    //going through lists od synonyms
    for(unsigned i = 0; i < ptrht->ht_size; i++){
        
        first = ptrht->ptr[i];

        if(first == NULL){
            continue;
        }
        
        //list of synonyms is not empty
        while(first != NULL){
            tmp = first;
            first = first->next;
            free(tmp->key);
            ptrht->dispose_func_ptr(tmp->data);
            free(tmp);
        }
        ptrht->ptr[i] = NULL;
        
    }

    ptrht->n_items = 0;
}

/**
 * @brief Frees the whole table 
 * @param ptrht hash table
 */

void ht_free(tHTable *ptrht){
    
    ht_clear_all(ptrht);  
    free(ptrht); 
}
/**
 * @brief functions for disposing test structure
 * @param data to be disposed
 * @pre data != NULL
 */
void dispose_func(tData data){

    free(((struct test *)(data))->str); 
    free((struct test *)(data));
    return ;
}

struct test * create_test(int x, char * str){

    struct test * tmp;
    tmp = malloc(sizeof(struct test));
    tmp->str = malloc(sizeof(char)*(strlen(str)+1));
    strcpy(tmp->str, str);
    tmp->x = x;

    return tmp; 
}

