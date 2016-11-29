class Main {
	static void run() {

		int a = 3;
		int b = 7;
		int c = 8;

		int count_1 = vypocet_1 (a,b,c);
		ifj16.print("Vysledok =" +count_1);
	}

	static int vypocet_1 (int p, int q, int r) {
		vypocet_2 (p, q, r);
		return 1;
	}

	static int vypocet_2 (int s, int u, int v) {
		vypocet_1 (s, u, v);
		return 1;
	}
}
