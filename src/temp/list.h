/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Martin Marušiak, xmarus07
 */

#ifndef list_H
#define list_H

typedef struct tElem {
        void *data;
        struct tElem *ptr;
} *tElemPtr;

typedef struct {
        tElemPtr First;
        tElemPtr Act;
        tElemPtr Last;
        void (*dispose_fun)(void *);
} tList;

void InitList(tList *, void (*dispose_fun)(void *));
void DisposeList(tList *);
void InsertFirst(tList *, void *);
void InsertLast(tList *, void *);
#define First_M(List) List->Act = List->First
void First(tList *);
void Last(tList *);
#define Last_M(List) List->Act = List->Last
void CopyFirst(tList *, void **);
void CopyLast(tList *, void **);
#define Copy_M(List, val) val = List->Act->data
void Actualize(tList *, void *);
#define Succ_M(List) (List->Act = List->Act->ptr)
#define Active_M(List) (List->Act != NULL)
#define GetActiveElem_M(List) (List->Act)
#define SetActiveElem_M(List, elem) List->Act = elem
#define GetLastElem_M(List) (List->Last)
void Succ(tList *);
int Active(tList *);
tElemPtr GetActiveElem(tList * L);
void SetActiveElem(tList * L, tElemPtr e);
tElemPtr GetLastElem(tList * L);
#endif
