#include <stdio.h>
#include <string.h>

#define MAXCHAR 256

void sort (char []);
void quickSort (char[], int, int);
int partition (char[], int, int);


void main() {
    char a[] = "tamara";        
	sort (a);

    printf("%s\n", a);
}

void sort (char a[]) {

    quickSort (a, 0, (strlen(a)-1));        
}

void quickSort( char a[], int left, int right) {

    int j;

    if(left < right ) {

        j = partition( a, left, right);
        quickSort( a, left, j-1);
        quickSort( a, j+1, right);
    }
	
}

int partition(char a[], int left, int right) {

    int pivot, i, j, t;
    pivot = a[left];
    i = left; 
    j = right+1;
		
    while(1) {

   	    do ++i; while(a[i] <= pivot && i <= right);
   	    do --j; while(a[j] > pivot);
   	    if(i >= j) 
            break;
   	    
        t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
   
    t = a[left]; 
    a[left] = a[j]; 
    a[j] = t;
    
    return j;
}

int MAX(int a, int b) {
    return (a > b ? a : b);
}

int MIN(int a, int b) {
    return (a < b ? a : b)
}

void compute_jumps(char *p, int jump[]) {

    int length_p = strlen(p) - 1;

    for(int k = 0; k < MAXCHAR; k++) {
        jump[k] = length_p;
    }

    for(int k = 0; k < length_p; k++) {
        jump[(unsigned char) p[k]] = length_p - k;
    }
}

void compute_match_jump(char *p, int match_jump[]) {

    int q = 0;
    int qq = 0;
    int backup[MAXCHAR];
    int m = strlen(p) - 1;

    for (int i = 0; i < m; i++) {
        match_jump[i] = 2 * m - i;
    }

    i = m;
    q = m + 1;

    while (i > 0) {
        backup[i] = q;
        while ((q <= m) && (p[i] != p[q])) {
            match_jump[q] = MIN(match_jump[q], m - i);
            q = backup[q];
        }
        i--;
        q--;
    }

    for (i = 1; i < q; i++) {
        match_jump[i] = MIN(match_jump[i], m + q - i);
    }

    qq = backup[q];

    while (q < m) {
        while (q < qq) {
            match_jump[q] = MIN(match_jump[q], qq - q + m);
            q++;
        }
        qq = backup[qq];
    }
}

void BMA(){
    /*doplnim do vecera*/
}