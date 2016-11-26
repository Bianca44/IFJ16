class Main {
    static int c = 5 / true; 
    //static int po = 5;
    //static int ll = 5+5 ;
    //static boolean g = 3 < 4.5 ;
    //static boolean mm = (5 != 5) && true;
    //static boolean dd = 3.5 != 6;
    //static double t = 6.5 + 3.5;
    //static double r = 6.5 - 3.5;
   // static double f = 6.5 - 3;
   // static double z = 6 + 3.5;
    static double j = 5 * 5.5;
   // static double m = 25.5 / 2;
   // static double d = (25.5 / 2.5) * 10;
   // static boolean v = 2 < 9;
   // static boolean ko = true ;
   // static boolean bbb = false ;
   // static boolean fff = bbb || ko;
   // static double m = "ahoj" - 3.5;
   // static String k =  5.36 + "ahoj" + "devet" ;
	//static int g = 3;
	//static int j = 2*3+2*2;
	/*static int m = 5+5;*/
    static void run () {
        // int i = 0;
        //int x = 0;
        //while (i != 10) i = i + 5;
        
        
        //int i = 2;
         //   ifj16.print("test" + i);
    	ifj16.print("Hello world" + "42" + 42 + "\n");
    	int f = 2*2+2*2;
    	ifj16.print(j);
    	ifj16.print(Main.j);
    	int a;
    	a = aa(true, "odkaz z main runu\n");
        ifj16.print(a);
		ifj16.print(j);
	}

/*	static void pp() {        ifj16.print("Kokotina");
    }*/

    static int aa(boolean e, String msg) {
    	ifj16.print("druha funkcia\n");
    	ifj16.print(e + "\n");
    	ifj16.print(msg);
    	return 5;
    }
}
