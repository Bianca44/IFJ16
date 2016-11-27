#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"
#include "error_codes.h"
//TODO
unsigned hash_code(const tKey key, unsigned ht_size) {
    unsigned h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *) key; *p != '\0'; p++) {
	    h = 65599 * h + *p;
    }
    return h % ht_size;
}

tHTable *ht_init(unsigned ht_size, unsigned (*hash_code_ptr) (const tKey, unsigned),
		 void (*dispose_func_ptr) (tData data)) {

    tHTable *new_table;
    //allocating space for table
    if ((new_table = malloc(sizeof(tHTable) + ht_size * sizeof(tSTitem *))) == NULL) {
        fprintf(stderr,"Could not allocate memory\n");
	    exit(INTERNAL_INTERPRET_ERROR);
    }

    /*setting values of new hash table */
    new_table->ht_size = ht_size;
    new_table->hash_code_ptr = hash_code_ptr;
    new_table->dispose_func_ptr = dispose_func_ptr;
    new_table->n_items = 0;

    for (unsigned i = 0; i < ht_size; i++) {
	new_table->ptr[i] = NULL;
    }

    return new_table;
}

tSTitem *ht_search(tHTable * ptrht, const tKey key) {
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    //accesing list of synonyms
    tSTitem *first = ptrht->ptr[index];

    while (first != NULL) {
	    if (strcmp(first->key, key) == 0) {
	        return first;
	    }

	    first = first->next;
    }
    // not found
    return NULL;
}

void ht_insert(tHTable * ptrht, const tKey key, tData data) {

    tSTitem *item = ht_search(ptrht, key);
    if (item == NULL) {
	    //getting index
	    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
	    //accesing list of synonyms
	    tSTitem *first = ptrht->ptr[index];

	    //adding item
	    tSTitem *new;

	    if ((new = malloc(sizeof(tSTitem))) == NULL) {
            fprintf(stderr,"Could not allocate memory\n");
	        exit(INTERNAL_INTERPRET_ERROR);
	    }

	    if ((new->key = malloc(sizeof(char) * (strlen(key) + 1))) == NULL) {
	        free(new);
            fprintf(stderr,"Could not allocate memory\n");
	        exit(INTERNAL_INTERPRET_ERROR);
	    }

	    strcpy(new->key, key);
	    new->data = data;
	    //adding to beginning
	    new->next = first;
	    ptrht->ptr[index] = new;

    	ptrht->n_items += 1;
        }
    else {
	    //actualizing item
	    item->data = data;
    }

}

tData ht_read(tHTable * ptrht, const tKey key) {

    tSTitem *item;

    if ((item = ht_search(ptrht, key)) == NULL) {
        return NULL;
    }

    return item->data;
}

void ht_delete(tHTable * ptrht, tKey key) {
    //getting index
    unsigned index = ptrht->hash_code_ptr(key, ptrht->ht_size);
    //accesing list of synonyms
    tSTitem *tmp = ptrht->ptr[index];
    tSTitem *tmp_prev = tmp;

    while (tmp != NULL) {
	    if (strcmp(tmp->key, key) == 0) {
	    //item to be deleted is first
	        if (tmp == tmp_prev) {
		        ptrht->ptr[index] = tmp->next;
	        }
            else {
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

void ht_clear_all(tHTable * ptrht) {

    tSTitem *first, *tmp;
    //going through lists od synonyms
    for (unsigned i = 0; i < ptrht->ht_size; i++) {

        first = ptrht->ptr[i];

        if (first == NULL) {
            continue;
        }
        //list of synonyms is not empty
        while (first != NULL) {
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

void ht_free(tHTable * ptrht) {

    ht_clear_all(ptrht);
    free(ptrht);
}

void quick_sort(char *str, int left, int right) {
    int indexes[2];
    partition(str, left, right, indexes);

    int i = indexes[0];
    int j = indexes[1];

    if (left < j) {
	quick_sort(str, left, j);
    }
    if (i < right) {
	quick_sort(str, i, right);
    }
}

void partition(char *str, int left, int right, int indexes[]) {
    int median = 0;
    int i = left;
    int j = right;

    median = str[(i + j) / 2];

    do {
	while (str[i] < median) {
	    i++;
	}

	while (str[j] > median) {
	    j--;
	}

	if (i <= j) {
	    int temp = str[i];
	    str[i] = str[j];
	    str[j] = temp;

	    i++;
	    j--;
	}
    }

    while (i <= j);

    indexes[0] = i;
    indexes[1] = j;
}

void compute_char_jump(char *p, int char_jump[]) {

    int length_p = strlen(p) - 1;

    for (int k = 0; k < ASCII_SIZE; k++) {
	char_jump[k] = length_p;
    }

    for (int k = 0; k <= length_p; k++) {
	char_jump[(int) p[k]] = length_p - k;
    }
}

void compute_match_jump(char *p, int match_jump[]) {

    int length_p = strlen(p);
    int k = 0;
    int q = 0;
    int qq = 0;
    int backup[ASCII_SIZE];

    for (k = 0; k < length_p + 1; k++) {
	match_jump[k] = 2 * length_p - k;
    }

    k = length_p;
    q = length_p + 1;

    while (k > 0) {
	backup[k] = q;
	while (q <= length_p && p[k - 1] != p[q - 1]) {
	    match_jump[q] = MIN(match_jump[q], length_p - k);
	    q = backup[q];
	}
	k--;
	q--;
    }

    for (k = 0; k < q + 1; k++) {
	match_jump[k] = MIN(match_jump[k], length_p + q - k);
    }

    qq = backup[q];

    while (q <= length_p) {
	while (q <= qq) {
	    match_jump[q] = MIN(match_jump[q], qq - q + length_p);
	    q++;
	}
	qq = backup[qq];
    }
}

int find_bma(char *p, char *t) {
    // BMA
    int m = strlen(p);
    int n = strlen(t);
    int j = m;
    int char_jump[ASCII_SIZE];
    int match_jump[m];

    compute_char_jump(p, char_jump);
    compute_match_jump(p, match_jump);

    while (j <= n && m > 0) {
	if (t[j - 1] == p[m - 1]) {
	    j--;
	    m--;
	} else {
	    j = j + MAX(char_jump[(int) t[j]], match_jump[m]);
	    m = strlen(p);
	}
    }
    if (m == 0) {
	// Java indexes from 0
	return j;
    } else {
	return -1;
    }
}
