#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "symbol_table.h"
//potrebne preto zasobnik, enum tokenov, treba dopnit
// $, "<" a E do enumu alebo spravit vlastny enum

enum Terminals{
    P_ADD,    // +
    P_SUB,    // -
    P_MUL,    // *
    P_DIV,    // /
    P_LB,     // (
    P_RB,     // )
    P_LESS,   // <
    P_GRT,    // >
    P_GRE,    // >=
    P_LESSE,  // <=
    P_EQL,    // ==
    P_NEQL,   // !=
    P_AND,    // &&
    P_OR,     // ||
    P_ID,     // id
    P_LIT,    // literal
    P_NOT,    // !
    P_ENDMARK,// $
    P_EXPR,   // E 
    P_HANDLE // <
};

//teoreticky uchovavat typ operacie, typ premennej atd, vytvorit este polozku zasobnika

typedef struct{
	struct PStack_item *LPtr;
    struct PStack_item *RPtr;
    enum Terminals term;  //typ terminalu  
    int t_type;  
}PStack_item;

typedef struct{
    PStack_item *top;
    PStack_item *first;

}PStack;



PStack *PSInit();
void PSPush(PStack *P,enum Terminals term);
void PSPop(PStack *P);
int  PSTopTerm(PStack *P); // zistenie prveho terminalu
PStack_item *PSTopTermPtr(PStack *P); // ukazatel na polozku s najvrchnejsim terminalom
void PSDestroy(PStack *P);
bool is_top_terminal(int term);
void insert_handle(PStack *P,PStack_item *item);
void PSPrint(PStack *P);

