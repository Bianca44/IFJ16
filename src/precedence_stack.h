#include "scanner.h" 
//potrebne preto zasobnik, enum tokenov, treba dopnit
// $, "<" a E do enumu alebo spravit vlastny enum
#define STACK_SIZE 30

typedef enum PSError{
    INIT_ERROR, 
    ALLOC_ERROR,
    INSERT_ERROR
}


typedef enum Terminals{
    P_ADD,    // +
    P_SUB,    // -
    P_MUL,    // *
    P_DIV,    // /
    P_LB,     // (
    P_RB,     // )
    //P_COMMA,  // ,
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
    P_ENDMARK,    // $
   // P_ASSIGN, // =
    P_EXPR,   // E 
    P_L,      // <
};    

//teoreticky uchovavat typ operacie, typ premennej atd, vytvorit este polozku zasobnika

typedef struct{
	struct PStack *LPtr;
    struct PStack *RPtr;
    token_t *t_type;   //typ tokenu
    Terminals term;  //typ terminalu    
}PStack_item;

typedef struct{
    PStack_item *top;
    PStack_item *first;

}PStack;



PSError PSInit();
void PSPush(PStack *P,Terminal term);
void PSPop(PStack *P);
int  PSTopTerm(PStack *P); // zistenie prveho terminalu
void PSDestroy(PStack *P);




