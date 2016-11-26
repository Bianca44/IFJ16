#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef NDEBUG

#define d_print(str, ...)
#define d_message(str)
#define d_inst_name()

#define d_int(i)
#define d_dob(d)
#define d_bol(b)
#define d_str(str)
#define d_intp(i, ptr)
#define d_dobp(d, ptr)
#define d_bolp(b, ptr)
#define d_strp(str, ptr)
#define d_ptr(p)
#define d_tVarPtr(s)


#else

#define d_print(str, ...) fprintf(stderr, __FILE__ ":%u: "str "\n", __LINE__, __VA_ARGS__)
#define d_message(str) fprintf(stderr, __FILE__ ":%u: "str "\n", __LINE__)
#define d_inst_name() fprintf(stderr, __FILE__ ":%u: %50s\n",__LINE__,__func__)

#define d_int(i) fprintf(stderr, __FILE__ ":%u: %50d\n", __LINE__, i)
#define d_dob(d) fprintf(stderr, __FILE__ ":%u: %50f\n", __LINE__, d)
#define d_bol(b) fprintf(stderr, __FILE__ ":%u: %50d\n", __LINE__, b)
#define d_str(s) fprintf(stderr, __FILE__ ":%u: %50s\n", __LINE__,s)
#define d_ptr(p) fprintf(stderr, __FILE__ ":%u: %50p\n", __LINE__,(void *)p)

#define d_intp(i, ptr) fprintf(stderr, __FILE__ ":%u: INT: %50d\t|%p|\n", __LINE__, i, (void *)ptr)
#define d_dobp(d, ptr) fprintf(stderr, __FILE__ ":%u: DOUBLE: %50f\t|%p|\n", __LINE__, d, (void *)ptr)
#define d_bolp(b, ptr) fprintf(stderr, __FILE__ ":%u: BOOL: %50d\t|%p|\n", __LINE__, b, (void *)ptr)
#define d_strp(s, ptr) fprintf(stderr, __FILE__ ":%u: STRING: %50s\t|%p|\n", __LINE__,s, (void *)ptr)


#define d_tVarPtr(var) (var != NULL ? \
                        (var->data_type == INT ? (d_intp(var->i, var)) : \
                         ((var->data_type == DOUBLE) ? (d_dobp(var->d, var)) : \
                    (((var->data_type == BOOLEAN) ? (d_bolp(var->b, var)) : \
                      ((((var->data_type == STRING) ? (d_strp(var->s, var)) : (d_strp("SPECIAL", var))))))))) : \
                        (d_ptr(NULL)))
#endif

#endif              //DEBUG_H
