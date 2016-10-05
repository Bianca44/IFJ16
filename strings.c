#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"

bool init_string(string str) {
        if ((str->data = malloc(STRING_INITIAL_ALLOCATION * sizeof(char))) == NULL) {
                return false;
        }
        str->data[0] = '\0';
        str->length = 0;
        str->allocated_size = STRING_INITIAL_ALLOCATION;
        return true;

}

void clear_string(string str) {
        free(str->data);
        str->data[0] = '\0';
        str->length = 0;
        str->allocated_size = 0;
}



void remove_last_char(string str) {
        if (str->length == 0) {
                return;
        }
        str->length--;
        str->data[str->length] = '\0';
}

bool append_char(string str, char c) {
        if (str->length + 1 >= str->allocated_size) {
                int new_size = (str->length +
                 STRING_INITIAL_ALLOCATION) *
                sizeof(char);
                if ((str->data = (char *) realloc(str->data,
                                              new_size)) == NULL) {
                        return false;
                }
                str->allocated_size = new_size;
        }
        str->data[str->length] = c;
        str->length++;
        str->data[str->length] = '\0';
        return true;
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

void QuickSort(char *text, int left, int right) {
        int indexes[2];

        partition(text, left, right, indexes);

        int i = indexes[0], j = indexes[1];

        if (left < j) {
                QuickSort(text, left, j);
        }
        if (i < right) {
                QuickSort(text, i, right);
        }
}

void sort(string str) {
        QuickSort(str->data, 0, strlen(str->data) - 1);

}

void computeCharJump(char *pattern, int charJump[]) {
        int str_length = strlen(pattern) - 1;

        for (int ch = 0; ch < 256; ch++) {
                charJump[ch] = str_length;
        }
        for (int k = 0; k <= str_length; k++) {
                charJump[(int) pattern[k]] = str_length - k;
        }
}

#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#define MAX(a,b) (((a)>(b)) ? (a) : (b))

void computeMatchJump(char *pattern, int matchJump[]) {

        int k, q, qq, m;
        int backup[256];

        k = q = qq = m = 0;

        m = strlen(pattern) - 1;

        for (int k = 0; k <= m; k++) {
                matchJump[k] = 2 * m - k;
        }

        k = m;
        q = m + 1;

        while (k > 0) {
                backup[k] = q;
                while ((q <= m) && (pattern[k] != pattern[q])) {
                        matchJump[q] = MIN(matchJump[k], m - k);
                        q = backup[q];
                }
                k--;
                q--;
        }

        for (k = 0; k <= q; k++) {
                matchJump[k] = MIN(matchJump[k], m + q - k);
        }

        qq = backup[q];

        while (q < m) {
                while (q <= qq) {
                        matchJump[q] = MIN(matchJump[q], qq - q + m);
                        q++;
                }
                qq = backup[qq];
        }

}

void find(string str, char *pattern) {
        int j, k, str_length, max;

        j = k = str_length = 0;

        int matchJump[256];
        int charJump[256];


        computeCharJump(pattern, charJump);

        computeMatchJump(pattern, matchJump);

        j = k = max = strlen(pattern) - 1;

        while ((j <= str->length) && (k > 0)) {
                if (str->data[j] == pattern[k]) {
                        j--;
                        k--;
                } else {
                        j = j + MAX(charJump[(int) str->data[j]], matchJump[k]);
                        k = strlen(pattern) - 1;
                }

        }

        printf("origo: %s, search: %s, k = %d \n", str->data, pattern, k);

        if (k == 0) {
                printf("found na indexe %d\n", j + 1);
        } else {
                printf("not found\n");
        }
}
