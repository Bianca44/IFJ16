#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#define INTERPRET_OK 0

#define LEXICAL_ANALYSIS_ERROR 1
#define SYNTACTIC_ANALYSIS_ERROR 2

/* sémantická chyba v programu – nedefinovaná třída/funkce/proměnná, pokus o re-
definici třídy/funkce/proměnné, atd.*/
#define SEMANTIC_ANALYSIS_PROGRAM_ERROR 3

/* sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních
výrazech, příp. špatný počet či typ parametrů u volání funkce. */
#define SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR 4

/* ostatní sémantické chyby.*/
#define SEMANTIC_ANALYSIS_OTHER_ERROR 6

/* běhová chyba při načítání číselné hodnoty ze vstupu. */
#define RUN_INPUT_ERROR 7

/* běhová chyba při práci s neinicializovanou proměnnou.*/
#define RUN_UNINITIALIZED_VARIABLE_ERROR 8

/* běhová chyba dělení nulou.*/
#define RUN_DEVIDE_ZERO_ERROR 9

/* ostatní běhové chyby. */
#define RUN_OTHER_ERROR 10

/* interní chyba interpretu tj. neovlivněná vstupním programem (např. chyba a-
lokace paměti, chyba při otvírání souboru s řídicím programem, špatné parametry
příkazové řádky atd.). */

#define INTERNAL_INTERPRET_ERROR 99

#endif // ERROR_CODES_H
