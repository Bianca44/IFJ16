/**
 * @file htable.h
 * @author Martin Marušiak
 * @brief header file of hash table
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H
/*testing list*/
typedef struct htab_listitem{
    char *key;
    unsigned data;
    struct htab_listitem *next;
} * t_htab_listitem_ptr;


/* hash table */
typedef struct hash_table{
    unsigned htab_size;
    unsigned (*hash_fun_ptr)(const char *str, unsigned htab_size);
    unsigned n_items;
    void *ptr[];
} t_hash_table;

/* funcions implementing hash table */
unsigned int hash_function(const char *str, unsigned htab_size); 
t_hash_table * hash_table_init(unsigned size);
void hash_table_free(t_hash_table *t, void (* dispose_func)(void *));
void * hash_table_search_add(t_hash_table *t, void * (*list_find_add)(void *), const char *key);
/*functions implementing lists*/
//uvolnenie, alokovanie
void * find_add_listitem(void * list);
void dispose_htab_listitem(void * list);
#endif /*HASH_TABLE_H*/

