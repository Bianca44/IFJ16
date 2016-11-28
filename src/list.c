#include <stdio.h>
#include <stdlib.h>
#include "list.h"

//TODO ial, komenty, odstranit nepotrebne

void InitList(tList * L, void (*dispose_fun) (void *)) {

    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
    L->dispose_fun = dispose_fun;

}

void DisposeList(tList * L) {

    while (L->First != NULL) {
	tElemPtr tmp;
    /* save the pointer that points at the first (disposing) item */
	tmp = L->First;
    /* shift */
	L->First = L->First->ptr;	
	L->dispose_fun(tmp->data);
	free(tmp);
    }
    /* return to the initialization state */
    L->Last = NULL;
    L->Act = NULL;
}

void InsertFirst(tList * L, void *val) {

    tElemPtr item;
    /* the control of memory allocation */
    if ((item = malloc(sizeof(struct tElem))) == NULL) {
	//TODO
	return;
    }
    /* setting the previous item and the next item */
    //item->lptr = NULL;
    item->ptr = L->First;
    item->data = val;

    /* in case of empty list, add the first and the last intem at the same time */
    if (L->First == NULL) {	
	L->Last = item;
    }

    L->First = item;
}

void InsertLast(tList * L, void *val) {

    tElemPtr item;
    /* the control of memory allocation */
    if ((item = malloc(sizeof(struct tElem))) == NULL) {
	//TODO
	return;
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

void First(tList * L) {

    L->Act = L->First;

}

void Last(tList * L) {

    L->Act = L->Last;
}


void CopyFirst(tList * L, void **val) {

    if (L->First == NULL) {
	return;
    }

    *val = L->First->data;
}

void CopyLast(tList * L, void **val) {

    if (L->First == NULL) {
	//TODO
	return;
    }

    *val = L->Last->data;
}

void Copy(tList * L, void **val) {

    if (L->Act == NULL) {
	//TODO
	return;
    }
    *val = L->Act->data;
}

void Actualize(tList * L, void *val) {

    if (L->Act != NULL) {
	L->Act->data = val;
    }
}

void Succ(tList * L) {

    if (L->Act != NULL) {
	L->Act = L->Act->ptr;
    }
}

int Active(tList * L) {

    return (L->Act != NULL);
}

tElemPtr GetActiveElem(tList * L) {
    return L->Act;
}

void SetActiveElem(tList * L, tElemPtr e) {
    L->Act = e;
}

/* returns last elemt of list */
tElemPtr GetLastElem(tList * L) {
    return L->Last;
}
