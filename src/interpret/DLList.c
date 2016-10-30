#include <stdio.h>
#include <stdlib.h>
#include "DLList.h"

//TODO ial, komenty, odstranit nepotrebne

void DLInitList (tDLList *L, void (*dispose_fun)(void *)) {

    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
    L->dispose_fun = dispose_fun;

}

void DLDisposeList (tDLList *L) {

    while(L->First != NULL){
        tDLElemPtr tmp;
        tmp = L->First; //odlizime si ukazatel na prvy prvok - mazany
        L->First = L->First->rptr; //posunieme sa
        L->dispose_fun(tmp->data);
        free(tmp);
    }
    //navrat do incializacneho stavu
    L->Last = NULL;
    L->Act = NULL;	
}

void DLInsertFirst (tDLList *L, void * val) {

    tDLElemPtr item; 
    //kontrola alokovania pamati
 	if((item = malloc(sizeof(struct tDLElem))) == NULL){
        //TODO
        return;
    }
    //nastavnie predchodcu a nasledovnika
    item->lptr = NULL;
    item->rptr = L->First;
    item->data = val;
    
    if(L->First != NULL){
        //nastavnie noveho prvku ako predchodcu minuleho
        L->First->lptr = item;
    }
    else{ // ak bol zaznam prazdny, je pridavany prvy aj posledny
        L->Last = item;
    }

    L->First = item;
}

void DLInsertLast(tDLList *L, void * val) {

    tDLElemPtr item; 
    //kontrola alokovania pamati
 	if((item = malloc(sizeof(struct tDLElem))) == NULL){
        //TODO
        return;
    }
    //nastavnie predchodcu a nasledovnika
    item->lptr = L->Last;
    item->rptr = NULL;
    item->data = val;
    
    if(L->Last != NULL){ 
    //nastavnie noveho prvku ako nasledovika minuleho
        L->Last->rptr = item;
    }
    else{ // ak bol zaznam prazdny, je pridavany prvy aj posledny
        L->First = item;
    }

    L->Last = item;
}

void DLFirst (tDLList *L) {

    L->Act = L->First;

}

void DLLast (tDLList *L) {

    L->Act = L->Last;
}

//?? *val
void DLCopyFirst (tDLList *L, void **val) {

    if(L->First == NULL){
        //TODO
        return;
    }	
	
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, void **val) {

    if(L->First == NULL){
        //TODO
        return;
    }	
	
    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {

    if(L->First != NULL){ //ak nie je prazdny
        if(L->First == L->Act) //ak je aktualny
            L->Act = NULL;
        tDLElemPtr tmp = L->First; //uchavame si ukazatel na mazany prvok
        if(L->First == L->Last){ //ak je jedinym prvkom
            L->First = NULL;
            L->Last = NULL; 
        }
        else {
            //nastavime novy prvy prvok
            L->First = L->First->rptr;
            L->First->lptr = NULL;
        }
        L->dispose_fun(tmp->data);
        free(tmp);
    }
}	

void DLDeleteLast (tDLList *L) {

    if(L->First != NULL){ //ak nie je prazdny
        if(L->Last == L->Act) //ak je aktualny
            L->Act = NULL;
        tDLElemPtr tmp = L->Last; //uchovame si ukazatel na mazany prvok
        if(L->First == L->Last){ //ak je jedinym prvkom
            L->First = NULL;
            L->Last = NULL; 
        }
        else {
            //nastavime novy posledny prvok
            L->Last = L->Last->lptr;
            L->Last->rptr = NULL;
        }
        L->dispose_fun(tmp->data);
        free(tmp);
    }
}

void DLPostDelete (tDLList *L) {

    if(L->Act != NULL && L->Act->rptr != NULL){
        tDLElemPtr tmp;
        tmp = L->Act->rptr;
        L->Act->rptr = tmp->rptr;
        // ak mazany nie je posledny nastavime suseda, inak sa aktualny stane poslednym
        if(tmp == L->Last){
            L->Last = L->Act;
        }
        else{
            tmp->rptr->lptr = L->Act;
        }
        L->dispose_fun(tmp->data);
        free(tmp);
    }
}

void DLPreDelete (tDLList *L) {

    if(L->Act != NULL && L->Act->lptr != NULL){
        tDLElemPtr tmp;
        tmp = L->Act->lptr;
        L->Act->lptr = tmp->lptr;
        // ak mazany nie je prvy nastavime suseda, inak sa aktualny stane poslednym
        if(tmp == L->First){
            L->First = L->Act;
        }
        else{
            tmp->lptr->rptr = L->Act;
        }
        L->dispose_fun(tmp->data);
        free(tmp);
    }

}

void DLPostInsert (tDLList *L, void * val) {
   	
    if(L->Act != NULL){
        tDLElemPtr item;
        if((item = malloc(sizeof(struct tDLElem))) == NULL){
            //TODO
            return;
        }
        //nastavenie hodnot noveho prvku
        item->data = val;
        item->lptr = L->Act;
        item->rptr = L->Act->rptr;
        
        //nastavanie susedov
        L->Act->rptr = item;
        if(L->Act == L->Last){ 
            L->Last = item;
        }
        else{
            item->rptr->lptr = item;
        }
    }	

}

void DLPreInsert (tDLList *L, void * val) {

    if(L->Act != NULL){
        tDLElemPtr item;
        if((item = malloc(sizeof(struct tDLElem))) == NULL){
            //TODO
            return;
        }
        //nastavenie hodnot noveho prvku
        item->data = val;
        item->rptr = L->Act;
        item->lptr = L->Act->lptr;
        
        //nastavanie susedov
        L->Act->lptr = item;
        if(L->Act == L->First){ 
            L->First = item;
        }
        else{
            item->lptr->rptr = item;
        }
    }	
}

void DLCopy (tDLList *L, void **val) {
   
    if(L->Act == NULL){
        //TODO
        return;
    }   
    *val = L->Act->data;
}

void DLActualize (tDLList *L, void *val) {

	if(L->Act != NULL){
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {

    if(L->Act != NULL){
        L->Act = L->Act->rptr;
    }	
}


void DLPred (tDLList *L) {

    if(L->Act != NULL){
        L->Act = L->Act->lptr; 
    }
}

int DLActive (tDLList *L) {

    return (L->Act != NULL);
}
//TODO

tDLElemPtr DLActiveElem (tDLList *L) {
    return L->Act;
}

void DLSetActive (tDLList *L, tDLElemPtr e){
    L->Act = e;
}
