#include <stdio.h>

int fibonacci (int number) {

        int a;
        int b;
        int p1;
        int p2;
        if ((number == 1) || (number == 2)) {
            return 1;
        }

        else {
                p1 = number-1;
                a = fibonacci(p1);
                p2 = number-2;
                b = fibonacci(p2);
                return a+b;
        }

}
int main() {

            int n = 15;
            int i = 1;
            int x = 0;

            while (i<=n) {
                    x = fibonacci(i);
                    printf("%d\n",x);
                    i = i + 1;
            }


    
return 0;
}
