
//ERROR 9 delenie nulou
class Main {
	static void run () {
		int a = 100000;
		int b = 150000;
		int c = 1;
		int vysledok = 0;

		while (10000000 > c) {
			vysledok = a/b;
			b = b - 1; //po 15 cykle bude b=0 --> delenie nulou;
			c = c + 1;
		}
	}
}
