class Main
{
    static void run()
    {

        int i = 0;
        int x = 0;
        while (i != 10000000) {
            x = x + 5;
            i = i + 1;
        }
        ifj16.print(x);
        a(x);
    }

    static void a(double s) {
        ifj16.print(s);
    }

}
