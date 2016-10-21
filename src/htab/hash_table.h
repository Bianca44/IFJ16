/**
 * @file htable.h
 * @author Martin Marušiak
 * @brief header file of hash table
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H


/* key */
typedef char * tKey;
/* data */
typedef void * tData;

/*symbol table item*/
typedef struct tSTitem{
    tKey key;
    tData data;
    struct tSTitem *next;
} tSTitem;

/*test structure*/
struct test {
    int x;
    char *str;
};


/*hash table*/
typedef struct tHTable{
    unsigned ht_size; //size of table
    unsigned (*hash_code_ptr)(const tKey, unsigned ht_size); // hash function
    void (*dispose_func_ptr)(tData data); //function which will be used to free data
    unsigned n_items; // number of items in table TODO
    tSTitem *ptr[]; // flexible array member 
} tHTable;

/* funcions implementing hash table */
unsigned hash_code(const tKey key, unsigned ht_size);
tHTable * ht_init(unsigned ht_size, unsigned (*hash_code_ptr)(const tKey, unsigned),
                  void (*dispose_func_ptr)(tData data));
tSTitem * ht_search(tHTable * ptrht, const tKey key);
void ht_insert(tHTable * ptrht, const tKey key, tData data);   
tData ht_read(tHTable * ptrht, const tKey key);
void ht_delete (tHTable * ptrht, tKey key);
void ht_clear_all(tHTable *ptrht);
void ht_free(tHTable *ptrht);

/*functions for deleting data*/
void dispose_func(tData data);
/* create test */
struct test * create_test(int x, char * str);

#endif /*HASH_TABLE_H*/

