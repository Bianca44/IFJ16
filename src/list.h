#ifndef DLList_H
#define DLList_H

//TODO IAL
typedef struct tDLElem {
        void *data;
        struct tDLElem *ptr;
} *tElemPtr;

typedef struct {
    tElemPtr First;
    tElemPtr Act;
    tElemPtr Last;
    void (*dispose_fun)(void *);
} tList;

void InitList (tList *, void (*dispose_fun)(void *));
void DisposeList (tList *);
//void DLInsertFirst (tList *, void *);
void InsertLast(tList *, void *);
void First (tList *);
void Last (tList *);
void CopyFirst (tList *, void **);
void CopyLast (tList *, void **);
//void DLDeleteFirst (tList *);
//void DLDeleteLast (tList *);
//void DLPostDelete (tList *);
//void DLPreDelete (tList *);
//void DLPostInsert (tList *, void *);
//void DLPreInsert (tList *, void*);
void Copy (tList *, void **);
void Actualize (tList *, void *);
void Succ (tList *);
//void DLPred (tList *);
int Active (tList *);
void SetActiveElem(tList *L, tElemPtr e);
tElemPtr GetActiveElem(tList *L);
tElemPtr GetLastElem(tList *L);
#endif
