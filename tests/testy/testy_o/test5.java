//ERROR 4, zly pocet predavanych parametrov fcii

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

		int count_1 = vypocet_1 (a,b);
		ifj16.print("Vysledok =" +count_1);
	}

	static int vypocet_1 (int p, int q, int r) {

		while (p>=q) {
			p--;
		}

		while (r<=q) {
			r++;
		} 

		int ret = p+q+r;

		return ret;

	}
}

	