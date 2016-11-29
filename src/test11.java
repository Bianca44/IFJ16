class Main {
static void run () {
        int n = 2;
        int i = 1;
        int x = 0;

        while (i<=n) {
                x = fibonacci(i);
                ifj16.print("" +x);
                i = i + 1;
        }
}

static int fibonacci (int number) {

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
                p2 = number+1;
                b = fibonacci(p2);
                return a+b;
        }


}
}
