#ifndef IAL_H
#define IAL_H

void sort(char *str);
void quick_sort(char *str, int left, int right);
void partition(char *str, int left, int right, int indexes[]);
void compute_jumps(char *p, int jump[]);
void compute_match_jump(char* p, int match_jump[]);
int find(char *p, char *t);

#endif // IAL_H
