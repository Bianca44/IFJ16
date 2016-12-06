#ifndef ERROR_CODES_H
#define ERROR_CODES_H

/* No errors */
#define INTERPRET_OK 0

/* Lexical analysis error */
#define LEXICAL_ANALYSIS_ERROR 1

/* Syntactic analysis error */
#define SYNTACTIC_ANALYSIS_ERROR 2

/* Semantic error – not defined class/function/variable, class/function/variable redefinition attempt, etc. */
#define SEMANTIC_ANALYSIS_PROGRAM_ERROR 3

/* Semantic error of the type compatibility in arithmetic/string/relational expressions or
** the incorrect number/type of parameters when calling a function
**/
#define SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR 4

/* Other semantic errors */
#define SEMANTIC_ANALYSIS_OTHER_ERROR 6

/* Runtime error while getting a numerical value from the input */
#define RUN_INPUT_ERROR 7

/* Runtime error while working with non-initialised variable */
#define RUN_UNINITIALIZED_VARIABLE_ERROR 8

/* Runtime error when dividing by zero*/
#define RUN_DEVIDE_ZERO_ERROR 9

/* Other runtime errors */
#define RUN_OTHER_ERROR 10

/* Internal interpret error that is not influenced by the input program 
** (memory allocation error, file opening error, incorrect parameters from the command line,..)
**/

#define INTERNAL_INTERPRET_ERROR 99

#endif				// ERROR_CODES_H
