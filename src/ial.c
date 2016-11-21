#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"

unsigned hash_code(const tKey key, unsigned ht_size) {
        unsigned h=0;
        const unsigned char *p;
        for(p=(const unsigned char*)key; *p!='\0'; p++)
        {
                h = 65599*h + *p;
        }
        return h % ht_size;
}

tHTable * ht_init(unsigned ht_size, unsigned (*hash_code_ptr)(const tKey, unsigned),
                  void (*dispose_func_ptr)(tData data)){

        tHTable *new_table;
        //allocating space for table
        if((new_table = malloc(sizeof(tHTable) + ht_size * sizeof(tSTitem *))) == NULL) {
                return NULL; //error
        }

        /*setting values of new hash table*/
        new_table->ht_size = ht_size;
        new_table->hash_code_ptr = hash_code_ptr;
        new_table->dispose_func_ptr = dispose_func_ptr;
        new_table->n_items = 0;

        for(unsigned i=0; i < ht_size; i++) {
                new_table->ptr[i] = NULL;
        }

        return new_table;
}

tSTitem * ht_search(tHTable * ptrht, const tKey key){
        //getting index
        unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
        //accesing list of synonyms
        tSTitem *first = ptrht->ptr[index];

        while(first != NULL) {
                //TODO strcmp
                if(strcmp(first->key, key) == 0) {
                        return first;
                }

                first = first->next;
        }
        // not found
        return NULL;
}

void ht_insert(tHTable * ptrht, const tKey key, tData data){

        tSTitem * item = ht_search(ptrht, key);
        if(item == NULL) {
                //getting index
                unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
                //accesing list of synonyms
                tSTitem *first = ptrht->ptr[index];

                //adding item
                tSTitem * new;

                if((new = malloc(sizeof(tSTitem))) == NULL) {
                        return; //error
                }

                if((new->key = malloc(sizeof(char) * (strlen(key)+1))) == NULL) {
                        free(new);
                        return; //error
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

tData ht_read(tHTable * ptrht, const tKey key){

        tSTitem * item;

        if((item = ht_search(ptrht, key)) == NULL) {
                return NULL;
        }

        return item->data;
}

void ht_delete (tHTable * ptrht, tKey key){
        //getting index
        unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
        //accesing list of synonyms
        tSTitem * tmp = ptrht->ptr[index];
        tSTitem * tmp_prev = tmp;

        while(tmp != NULL) {
                //TODO strcmp
                if(strcmp(tmp->key, key) == 0) {
                        //item to be deleted is first
                        if(tmp == tmp_prev) {
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

void ht_clear_all(tHTable *ptrht){

        tSTitem *first, *tmp;
        //going through lists od synonyms
        for(unsigned i = 0; i < ptrht->ht_size; i++) {

                first = ptrht->ptr[i];

                if(first == NULL) {
                        continue;
                }

                //list of synonyms is not empty
                while(first != NULL) {
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

void ht_free(tHTable *ptrht){

        ht_clear_all(ptrht);
        free(ptrht);
}

void quick_sort(char *text, int left, int right) {
        int indexes[2];

        partition(text, left, right, indexes);

        int i = indexes[0], j = indexes[1];

        if (left < j) {
                quick_sort(text, left, j);
        }
        if (i < right) {
                quick_sort(text, i, right);
        }
}

void partition(char *text, int left, int right, int indexes[]) {
        int median = 0;

        int i = left;
        int j = right;

        median = text[(i + j) / 2];

        do {
                while (text[i] < median) {
                        i++;
                }

                while (text[j] > median) {
                        j--;
                }
                if (i <= j) {
                        int c = text[i];

                        text[i] = text[j];
                        text[j] = c;

                        i++;
                        j--;
                }
        }
        while (i <= j);

        indexes[0] = i;
        indexes[1] = j;
}


void compute_jumps(char *p, int char_jump[]) {

        int length_p = strlen(p) - 1;

        for(int k = 0; k < MAXCHAR; k++) {
                char_jump[k] = length_p;
        }

        for(int k = 0; k <= length_p; k++) {
                char_jump[(int) p[k]] = length_p - k;
        }
}

void compute_match_jump(char* p, int match_jump[]) {

        int k, q, qq, m;
        int backup[256];

        k = q = qq = m = 0;

        m = strlen(p) - 1;

        for (int k = 0; k <= m; k++) {
                match_jump[k] = 2 * m - k;
        }

        k = m;
        q = m + 1;

        while (k > 0) {
                backup[k] = q;
                while ((q <= m) && (p[k] != p[q])) {
                        match_jump[q] = MIN(match_jump[k], m - k);
                        q = backup[q];
                }
                k--;
                q--;
        }

        for (k = 0; k <= q; k++) {
                match_jump[k] = MIN(match_jump[k], m + q - k);
        }

        qq = backup[q];

        while (q < m) {
                while (q <= qq) {
                        match_jump[q] = MIN(match_jump[q], qq - q + m);
                        q++;
                }
                qq = backup[qq];
        }

}

int find_bma(char *p, char *t) {
        int j, k;
        j = k = 0;

        int char_jump[MAXCHAR];
        int match_jump[MAXCHAR];


        compute_jumps(t, char_jump);

        compute_match_jump(t, match_jump);

        j = k = strlen(t) - 1;

        int len = strlen(p);

        while ((j <= len) && (k > 0)) {
                if (p[j] == t[k]) {
                        j--;
                        k--;
                } else {
                        j = j + MAX(char_jump[(int) p[j]], match_jump[k]);
                        k = strlen(t) - 1;
                }

        }

        if (k == 0) {
                return j + 1;
        } else {
                return -1;
        }
}
