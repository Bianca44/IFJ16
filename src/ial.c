#include <stdio.h>
#include <string.h>
#include "ial.h"

#define MAXCHAR 256
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define MIN(a,b) (((a)<(b)) ? (a) : (b))

void sort (char *str);
void quick_sort(char *str, int left, int right);
void partition (char *str, int left, int right, int indexes []);
void compute_jumps(char *p, int jump[]);
void compute_match_jump(char* p, int match_jump[]);
void bma(char *p, char *t);

int main(void) {
        char retazec[] = "bcjdsbefuhen";
        sort(retazec);
        printf("Sorted: %s\n", retazec);

        char t[]  = "ABBABCDABC";
        char p[]  = "ABC";
        int pos = find(p, t);
        printf("%d\n", pos);
}

void sort(char *str) {
        quick_sort(str, 0, (strlen(str)-1));
}

void quick_sort(char *str, int left, int right) {
        int indexes[2];
        partition (str, left, right, indexes);

        int i = indexes[0];
        int j = indexes[1];

        if (left < j) {
                quick_sort(str, left, j);
        }
        if (right < i) {
                quick_sort(str, i, right);
        }
}

void partition (char *str, int left, int right, int indexes[]) {
        int median = 0;
        int i = left;
        int j= right;

        median = str[(i+j)/2];

        do {
                while (str[i] < median) {
                        i++;
                }

                while (str[j] > median) {
                        j--;
                }

                if (i<=j) {
                        int temp = str[i];
                        str[i] = str[j];
                        str[j] = temp;

                        i++;
                        j--;
                }
        }

        while (i<=j);

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

        int length_p = strlen(p);
        int k = 0;
        int q = 0;
        int qq = 0;
        int backup[MAXCHAR];

        for(k = 0; k < length_p + 1; k++) {
                match_jump[k] = 2 * length_p - k;
        }

        k = length_p;
        q = length_p + 1;

        while(k > 0) {
                backup[k] = q;
                while(q <= length_p && p[k-1] != p[q-1]) {
                        match_jump[q] = MIN(match_jump[q], length_p - k);
                        q = backup[q];
                }
                k--;
                q--;
        }

        for(k = 0; k < q + 1; k++) {
                match_jump[k] = MIN(match_jump[k], length_p + q - k);
        }

        qq = backup[q];

        while(q <= length_p ) {
                while(q <= qq ) {
                        match_jump[q] = MIN(match_jump[q], qq - q + length_p);
                        q++;
                }
                qq = backup[qq];
        }
}

int find(char *p, char *t) {
        // BMA
        int m = strlen(p);
        int n = strlen(t);
        int j = m;
        int char_jump[MAXCHAR];
        int match_jump[m];

        compute_jumps(p, char_jump);
        compute_match_jump(p, match_jump);

        while(j <= n && m > 0) {
                if( t[j-1] == p[m-1]) {
                        j--;
                        m--;
                }
                else {
                        j = j + MAX(char_jump[(int)t[j]], match_jump[m]);
                        m = strlen(p);
                }
        }
        if (m == 0) {
                return j + 1;
        } else {
                return -1;
        }
}
