class Main{
    static String x;
    static void run(){
       // x = readString();
       x = ifj16.readString();
       str(x);
    }

    static void str(String x){
        int i = ifj16.find(x,"a");
        ifj16.print(i);
    }
}
