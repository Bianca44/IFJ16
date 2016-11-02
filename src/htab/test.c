#include <stdio.h>
#include "hash_table.h"


int main() {
    
    tHTable *t;
    t = ht_init(11, hash_code, dispose_func);
    struct test * a = create_test(42, "Sasha");
    struct test *b, *c = create_test(42, "Nein");


    ht_insert(t, "X", a);
    ht_insert(t, "Y", c);
    b = ht_read(t, "X"); 
    printf("%d %s %d\n", b->x, b->str, t->n_items);
    ht_delete(t, "X");
    b = ht_read(t, "X"); 
    printf("%d\n",b);  

    printf("%d \n", t->n_items);

    b = ht_read(t, "Y"); 
    printf("%d %s %d\n", b->x, b->str, t->n_items);
    ht_delete(t, "X");
    b = ht_read(t, "X"); 
    printf("%d \n",b);  

    ht_free(t);

    return 0;
}
