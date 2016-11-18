#ifndef BUILTIN_H
#define BUILTIN_H
#include "symbol_table.h"

void sort(char *str);
int find(char *p, char *t);
void print(tVar *var);
int length(char *string);
int compare(char *s1, char *s2);
char * substr(char *s, int i, int n);
int read_int();
double read_double();
char * read_string();

#endif // BUILTIN_H
