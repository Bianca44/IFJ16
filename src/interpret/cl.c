#include <stdio.h>

inline int val(int x, int y){
    y = y*y;
    x = y / (x+1);
    return x + 1;
}


int main(){
   int x = 0;
   int y = 2;
    for(int i=0; i < 200000000; i++)
        x = val(x,y);
   printf("%d\n", x);     
    

    return 0;
}
