class Main
{
    /*static int i = 0;
    static int x = 0;*/
    static void run()
    {
        int a = testReturn(5);
        ifj16.print(a);
    }

    static int testReturn(int i) {
        if (i > 10) {
            return i - 10;
        }
    }
}
