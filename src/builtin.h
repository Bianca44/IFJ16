#ifndef BUILTIN_H
#define BUILTIN_H
#include "symbol_table.h"

void sort(char *str); //Quicksort
int find(char *p, char *t); //Boyer-Moore algorithm
void print(tVar * var); //prints values
int length(char *string); //returns the lenght of a string
int compare(char *s1, char *s2); //compares two strings
char *substr(char *s, int i, int n); //finds a substring of a given string
int read_int();
double read_double();
char *read_string();

#endif				// BUILTIN_H
