class Main
{
static void run()
{
        int a;
        ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
        a = ifj16.readInt();
        int vysl;
        int vysl = 1; // takto ide
        while (a > 0) {
                vysl = vysl * a;
                a = a - 1;
        }
        ifj16.print(vysl);
}
}
