#include <stdio.h>
#include <string.h>

void sort (char *str);
void quickSort (char *str, int left, int right);
void partition (char *str, int left, int right, int indexes []);


void main() {   
    char retazec [20] = "bcjdsbefuhen";
    sort(retazec);
    printf("%s\n", retazec);
}

void sort (char *str) {
    quickSort(str, 0, (strlen(str)-1));
}

void quickSort (char *str, int left, int right) {
    int index [2];
    partition (str, left, right, index);

    int i = index[0];
    int j = index[1];

    if (left < j) {
        quickSort(str, left, j);
    }
    if (right < i) {
        quickSort(str, i, right);
    } 
}

void partition (char *str, int left, int right, int indexes []) {
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
}
