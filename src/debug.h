#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef NDEBUG
#define d_print(str, ...)
#define d_message(str)
#define d_inst_name()
#else
#define d_print(str, ...) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__, __VA_ARGS__)
#define d_message(str) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__)
#define d_inst_name() fprintf(stderr, __FILE__":%u: %50s\n",__LINE__,__func__)
#endif

#endif //DEBUG_H
