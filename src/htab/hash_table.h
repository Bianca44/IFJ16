/**
 * @file htable.h
 * @author Martin Marušiak
 * @brief header file of hash table
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H
/*testing list*/
/* key */
typedef char * tKey;

typedef struct tHTitem{
    tKey key;
    unsigned data;
    struct tHTitem *next;
} tHTitem;


/* hash table */
typedef struct tHTable{
    unsigned ht_size; //size of table
    unsigned (*hash_code_ptr)(const tKey, unsigned ht_size); // hash function
    unsigned n_items; // number of items in table TODO
    void *ptr[]; // flexible array member 
} tHTable;

/* funcions implementing hash table */
unsigned hash_code(const tKey key, unsigned ht_size);
 
tHTable * ht_init(unsigned ht_size, unsigned (*hash_code_ptr)(const tKey, unsigned ht_size));
void * ht_search(tHTable * ptrht, const tKey key, void *(* item_search_ptr)(void *, tKey key));
void ht_insert(tHTable * ptrht, const tKey key, void * data, void (*item_insert_first_ptr)(void *, void *));

void item_insert_first(void * item, void * data);

void * ht_read(tHTable * ptrht, const tKey key, void * (* item_get_value_ptr)(void *),void *(* item_search_ptr)(void *, tKey key));

void * item_get_value(void *item);

void ht_Delete (tHTable *ptrht, const tKey key, void (*item_delete_ptr)(void *));
void item_delete(void *item);

void ht_clear_all(tHTable *ptrht, void (* dispose_func_ptr)(void *));
void ht_free(tHTable *ptrht, void (* dispose_func_ptr)(void *));
//void * hash_table_search_add(tHTable *ptrht, void * (*list_find_add)(void *), const tKey key);

/*functions implementing lists*/
//uvolnenie, alokovanie
void * item_search(void * item, tKey key);
void dispose_ht_item(void * list);
#endif /*HASH_TABLE_H*/

