class Main
{
static void run()
{
        int a; int vysl; int neg;
        ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
        a = ifj16.readInt();
        vysl = factorial(a);
        neg = 0 - vysl;
        ifj16.print("Vysledek: " + vysl);
        ifj16.print(" (zaporny: " + neg+ ")\n");
}
static int factorial(int n) // Definice funkce pro vypocet faktorialu
{
        int temp_result;
        int decremented_n = n - 1;
        while(n < 2) {
                return 1;
        }
        temp_result = factorial(decremented_n);
        temp_result = n * temp_result;
        return temp_result;
}
}
