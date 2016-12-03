/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Martin Marušiak, xmarus07
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "error_codes.h"
/* initializing list */
void InitList(tList * L, void (*dispose_fun) (void *)) {

    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
    L->dispose_fun = dispose_fun;

}
/* disposes all items from list */
void DisposeList(tList * L) {

    while (L->First != NULL) {
    	tElemPtr tmp;
        /* saving pointer to element that will be disposed */
	    tmp = L->First;
        /* setting fisrt to next element */
	    L->First = L->First->ptr;
        /* disposing content of element*/
	    L->dispose_fun(tmp->data);
        /* freeing element */
	    free(tmp);
    }
    /* return to the initialization state */
    L->Last = NULL;
    L->Act = NULL;
}
/* inserting element to the beginning of list */
void InsertFirst(tList * L, void *val) {

    tElemPtr item;
    /* memory allocation */
    if ((item = malloc(sizeof(struct tElem))) == NULL) {
        exit(INTERNAL_INTERPRET_ERROR);
    }
    /*setting successor and value of new element*/
    item->ptr = L->First;
    item->data = val;

    /* list is empty, Last = Fisrt = item */
    if (L->First == NULL) {
	    L->Last = item;
    }

    L->First = item;
}

void InsertLast(tList * L, void *val) {

    tElemPtr item;
    /* memory allocation */
    if ((item = malloc(sizeof(struct tElem))) == NULL) {
        exit(INTERNAL_INTERPRET_ERROR);
    }
    /* setting the previous item and the next item */
    item->ptr = NULL;
    item->data = val;

    if (L->Last != NULL) {
	    /* setting a new item as the next one of the previous one */
	    L->Last->ptr = item;
        /* in case of empty list, add the first and the last intem at the same time */
    } else {
	    L->First = item;
    }

    L->Last = item;
}
/* sets activity to first element */
void First(tList * L) {

    L->Act = L->First;

}
/* sets activity to last element */
void Last(tList * L) {

    L->Act = L->Last;
}

/* copies value of first element in list */
void CopyFirst(tList * L, void **val) {

    if (L->First == NULL) {
	    exit(INTERNAL_INTERPRET_ERROR);
    }

    *val = L->First->data;
}
/* copies value of last element */
void CopyLast(tList * L, void **val) {

    if (L->First == NULL) {
        exit(INTERNAL_INTERPRET_ERROR);
    }

    *val = L->Last->data;
}
/* copies the value of active element */
void Copy(tList * L, void **val) {

    if (L->Act == NULL) {
	    exit(INTERNAL_INTERPRET_ERROR);
    }
    *val = L->Act->data;
}
/* actualize active element's value */
void Actualize(tList * L, void *val) {

    if (L->Act != NULL) {
	    L->Act->data = val;
    }
}
/* changes activity to successor */
void Succ(tList * L) {

    if (L->Act != NULL) {
	    L->Act = L->Act->ptr;
    }
}
/* returns true if list is active */
int Active(tList * L) {
    return (L->Act != NULL);
}
/* return active element */
tElemPtr GetActiveElem(tList * L) {
    return L->Act;
}
/* sets active element of list to e */
void SetActiveElem(tList * L, tElemPtr e) {
    L->Act = e;
}

/* returns last element of list */
tElemPtr GetLastElem(tList * L) {
    return L->Last;
}
