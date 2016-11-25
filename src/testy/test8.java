class Main {
	static void run (){
		Bool a = true;
		Bool b = false;

		if (a && b) {
			ifj2016.println("a && b");
		}

		else if (a || b) {
			ifj2016.println("a || b");
		}

		else {
			ifj2016.println("both are false");
		}

		if (a && a) {
			ifj2016.println("a is true");
		}

		else if (b && b) {
			ifj2016.println("b is true");
		}

		else {
			ifj2016.println("both are false");
		}
	}
}