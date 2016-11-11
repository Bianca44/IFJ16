#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef NDEBUG
#define d_print(str, ...)
#define d_message(str)
#else
#define d_print(str, ...) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__, __VA_ARGS__)
#define d_message(str) fprintf(stderr, __FILE__":%u: "str"\n", __LINE__)
#endif

#endif //DEBUG_H
