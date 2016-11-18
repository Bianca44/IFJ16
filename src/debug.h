#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef NDEBUG
#define d_print(str, ...)
#define d_message(str)
#define d_inst_name()
#define d_int(i) 
#define d_dob(d) 
#define d_bolr(b) 
#define d_str(str) 
#define d_ptr(p) 
#define d_tVar(s) 
#else
#define d_print(str, ...) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__, __VA_ARGS__)
#define d_message(str) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__)
#define d_inst_name() fprintf(stderr, __FILE__":%u: %50s\n",__LINE__,__func__)
#define d_int(i) fprintf(stderr, __FILE__":%u: %50d\n", __LINE__, i)
#define d_dob(d) fprintf(stderr, __FILE__":%u: %50f\n", __LINE__, d)
#define d_bol(b) fprintf(stderr, __FILE__":%u: %50d\n", __LINE__, b)
#define d_str(s) fprintf(stderr, __FILE__":%u: %50s\n", __LINE__,s)
#define d_ptr(p) fprintf(stderr, __FILE__":%u: %50p\n", __LINE__,p)
#define d_tVarPtr(var) (var != NULL ? \
                  (var->data_type == INT ? (d_int(var->i)) : \
                  ((var->data_type == DOUBLE) ? (d_dob(var->d)) : \
                  (((var->data_type == BOOLEAN) ? (d_bol(var->b)) : \
                  ((((var->data_type == STRING) ? (d_str(var->s)) : (d_str("SPECIAL"))))))))) : \
                  (d_ptr(NULL)))
#endif

#endif //DEBUG_H
