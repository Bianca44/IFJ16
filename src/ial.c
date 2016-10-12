#include <stdio.h>
#include <string.h>

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
