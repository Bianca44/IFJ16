/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Martin Marušiak, xmarus07
 */

/* this file contain macros for debug puporses */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef NDEBUG

#define d_print(str, ...) //as printf but with default \n 
#define d_message(str) //only pritns string - char *, \n is default
#define d_inst_name() // pritns name of function in which this macro is used

#define d_int(i) //prints integer
#define d_dob(d) //prints double
#define d_bol(b) //prints boolean
#define d_str(str) //prints string 
#define d_ptr(p) //prints value of pointer

/*these macros are being used in other macros*/
#define d_intp(i, ptr) //prints integer value and adress
#define d_dobp(d, ptr) //prints double value and adress
#define d_bolp(b, ptr) //prints boolean value and adress
#define d_strp(str, ptr) //prints string value
/*uses macros mentioned above and prints value and adress of tVar*/
#define d_tVarPtr(s)


#else

#define d_print(str, ...) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__, __VA_ARGS__)
#define d_message(str) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__)
#define d_inst_name() fprintf(stderr, __FILE__":%u: %50s\n",__LINE__,__func__)

#define d_int(i) fprintf(stderr, __FILE__":%u: %50d\n", __LINE__, i)
#define d_dob(d) fprintf(stderr, __FILE__":%u: %50f\n", __LINE__, d)
#define d_bol(b) fprintf(stderr, __FILE__":%u: %50d\n", __LINE__, b)
#define d_str(s) fprintf(stderr, __FILE__":%u: %50s\n", __LINE__,s)
#define d_ptr(p) fprintf(stderr, __FILE__":%u: %50p\n", __LINE__,(void *)p)

#define d_intp(i, ptr) fprintf(stderr, __FILE__":%u: INT: %50d\t|%p|\n", __LINE__, i, (void *)ptr)
#define d_dobp(d, ptr) fprintf(stderr, __FILE__":%u: DOUBLE: %50f\t|%p|\n", __LINE__, d, (void *)ptr)
#define d_bolp(b, ptr) fprintf(stderr, __FILE__":%u: BOOL: %50d\t|%p|\n", __LINE__, b, (void *)ptr)
#define d_strp(s, ptr) fprintf(stderr, __FILE__":%u: STRING: %50s\t|%p|\n", __LINE__,s, (void *)ptr)


#define d_tVarPtr(var) (var != NULL ? \
                  (var->data_type == INT ? (d_intp(var->i, var)) : \
                  ((var->data_type == DOUBLE) ? (d_dobp(var->d, var)) : \
                  (((var->data_type == BOOLEAN) ? (d_bolp(var->b, var)) : \
                  ((((var->data_type == STRING) ? (d_strp(var->s, var)) : (d_strp("SPECIAL", var))))))))) : \
                  (d_ptr(NULL)))
#endif

#endif //DEBUG_H
