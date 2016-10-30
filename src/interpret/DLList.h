#ifndef DLList_H
#define DLList_H

//TODO IAL
typedef struct tDLElem {
        void *data;
        struct tDLElem *lptr;
        struct tDLElem *rptr;
} *tDLElemPtr;

typedef struct {
    tDLElemPtr First;
    tDLElemPtr Act;
    tDLElemPtr Last;
    void (*dispose_fun)(void *);
} tDLList;

void DLInitList (tDLList *, void (*dispose_fun)(void *));
void DLDisposeList (tDLList *);
void DLInsertFirst (tDLList *, void *);
void DLInsertLast(tDLList *, void *);
void DLFirst (tDLList *);
void DLLast (tDLList *);
void DLCopyFirst (tDLList *, void **);
void DLCopyLast (tDLList *, void **);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostDelete (tDLList *);
void DLPreDelete (tDLList *);
void DLPostInsert (tDLList *, void *);
void DLPreInsert (tDLList *, void*);
void DLCopy (tDLList *, void **);
void DLActualize (tDLList *, void *);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);
void DLSetActive(tDLList *L, tDLElemPtr e);
tDLElemPtr DLActiveElem(tDLList *L);
#endif
