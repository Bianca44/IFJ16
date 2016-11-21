class Main
{
static void run()
{
        int a;
        ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
        a = ifj16.readInt();
        int vysl;
        vysl = 1;
        while (a > 0) {
                vysl = vysl * a;
                a = a - 1;
        }
        ifj16.print("Vysledek je: " + vysl + "\n");
}
}
