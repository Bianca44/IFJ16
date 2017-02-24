class Main {
	static void run() {

		int a;
		int b;
		int c;

		//ifj16.print("Zadejte 1.cislo: ");
        a = 7;//ifj16.readInt();
        //ifj16.print("Zadejte 2.cislo: ");
        b = 8;//ifj16.readInt();
        //ifj16.print("Zadejte 3.cislo: ");
        c = 9;//ifj16.readInt();

		if ((a>b) && (b>c)) {
			int count_1 = vypocet_1 (a,b,c);
			ifj16.print("Vysledok =" +count_1);
		}

		else {
			int count_2 = vypocet_2(a,b,c); 
			ifj16.print("Vysledok =" +count_2);
		}


	}

	static int vypocet_1 (int p, int q, int r) {

		while (p>=q) {
			p--;
		}

		while (r<=q) {
			r++;
		} 

		int ret = vypocet_2 (p,q,r); //vsetky by mali mat rovnaku hodnotu 
		return ret;

	}

	static int vypocet_2 (int s, int t, int u) {
		int ret_2 = s*t/u+2-4/5*(3+7)-(2*3-2);
		return ret_2;

	}
}

	