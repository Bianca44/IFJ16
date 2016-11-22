/* Program 3: Prace s retezci a vestavenymi funkcemi */
class Main
{
    static int x = 2;
static void run()
{
        String a;
        //int a;
        Game.play("h");
}
// end of static void run()
}
// end of class Main
class Game
{
static void play(String str)
{
        ifj16.print("Zadejte nejakou posloupnost vsech malych pismen a-h, ");
        ifj16.print("pricemz se pismena nesmeji v posloupnosti opakovat:");
        str = ifj16.readString();
        str = ifj16.sort(str);
        int cmp = ifj16.compare(str, "abcdefgh");
        cmp = 10;
        ifj16.print(cmp);
}
// end of static void play(String)
}
// end of class Game
