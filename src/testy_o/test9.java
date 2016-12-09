//Fibonacci rekurzia, iteracia

class Main {
	static void run () {
		//ifj.print("Zadaj pocet ");
		int n = 150;

		for (int i = 1; i<=n; i++){
			ifj16.print(fibonacci(i) + "");
		}
	}

	static int fibonacci (int number) {
		if (number == 1 || number == 2) {
			return 1;
		}

		return (fibonacci(number-1) + fibonacci(number-2));
	} 
}
