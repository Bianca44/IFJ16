class Main {
	static void run() {

		int a = 3;
		int b = 7;
		int c = 8;

		int count_1 = vypocet_1 (a,b,c);
		ifj16.print("Vysledok =" +count_1);
	}

	static int vypocet_1 (int p, int q, String r) {
		int rec = 0;
		while (p != 10000000) {
			r++;
			rec = vypocet_2 (p, q, r);
		}
		return (r+p);
	}

	static int vypocet_2 (int s, int u, int v) {
		int rec_2 = 0;
		while (v != 1000000) {
			s++;
			rec_2 = vypocet_1 (s, u, v);
		}
		return (v+u);
	}
}

	