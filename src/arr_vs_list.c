#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "symbol_table.h"
#include "instructions.h"

void dispose_inst2(void * a){

    free((tInst *)a);
        
}
#define SIZE 3
tList *list;
tInst * arr[SIZE] = {NULL, };
tInst *p;

void read_arr(){
    for(int i = 0; i < SIZE; i++){
    
        p = arr[i];
    }
}

void read_list(){
    printf("pred\n");

    printf("po\n");
    First(list);
    while(Active_M(list)){
     
        Copy_M(list, p);   
        Succ_M(list);

    }
}


int main(){
     list = malloc(sizeof(tList));
     InitList(list, dispose_inst2);
    goto SKOC;

    for(int i = 0; i < SIZE; i++){
       InsertFirst(list, NULL); 
    }
    read_arr();
    while(1){
        read_list();
        printf("dad\n");
    }
    SKOC: printf("sd\n");
    DisposeList(list);
    free(list);
}
