#include <stdio.h>
#include "hash_table.h"


int main() {
    
    tHTable *t;
    t = ht_init(10, hash_code); 
        
    //printf("table %d %d\n",t->htab_size, ((struct htab_listitem *)(t->ptr[0]))->data);
    //printf("%d\n",(int *)hash_table_search_add(t, find_add_listitem, "asdsfsdfdfd"));
    ht_clear_all(t, dispose_ht_item);
    ht_free(t, dispose_ht_item);
    return 0;
}
