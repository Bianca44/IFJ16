#include <stdio.h>
#include "hash_table.h"


int main() {
    
    t_hash_table *t;
    t = hash_table_init(10); 
        
    //printf("table %d %d\n",t->htab_size, ((struct htab_listitem *)(t->ptr[0]))->data);
    printf("%d\n",(int *)hash_table_search_add(t, find_add_listitem, "asdsfsdfdfd"));
    hash_table_free(t, dispose_htab_listitem);
    return 0;
}
