class Main
{
static void run()
{
        int a; int vysl; int neg;
        //ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
        a = 11;//ifj16.readInt();
        if (a < 0) {
                //ifj16.print("Faktorial nelze spocitat!\n");
        }
        else {
                vysl = factorial(a);
                neg = 0 - vysl;
                ifj16.print("Vysledek: " + vysl);
                ifj16.print(" (zaporny: " + neg+ ")\n");
        }
}
static int factorial(int n) // Definice funkce pro vypocet faktorialu
{
        int temp_result;
        int decremented_n = n - 1;
        int x = 1;
        int i = 0;
        if (n < 2) {
                return 1;
        }
        else {

                while (i != 1000000) {
                    x = daco(x);                   
                    i = i + x;
                }
                temp_result = factorial(decremented_n);
                temp_result = n * temp_result;
                return temp_result;
        }
}

static int daco(int q){
    String t = "faf";
    int x = d2(q);
    int y = 0;
    int z = x + y;
    return z;
    
}

static int d2(int z){

    return z;
}
}
