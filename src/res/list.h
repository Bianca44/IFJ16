#ifndef list_H
#define list_H

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
void InsertFirst (tList *, void *);
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
#define Copy_M(List, val) val = List->Act->data 
//void Copy (tList *, void **);
void Actualize (tList *, void *);
//void DLPred (tList *);
#define Succ_M(List) (List->Act = List->Act->ptr)
#define Active_M(List) (List->Act != NULL)
#define GetActiveElem_M(List) (List->Act)
#define SetActiveElem_M(List, elem) List->Act = elem
#define GetLastElem_M(List) (List->Last)
void Succ(tList *);
int Active(tList *);
tElemPtr GetActiveElem(tList *L);
void SetActiveElem(tList *L, tElemPtr e);
tElemPtr GetLastElem(tList *L);
#endif
