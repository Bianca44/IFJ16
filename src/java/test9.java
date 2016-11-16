class Main {    static void run () {
                        int a;
                        {
                            {}
                            a = 5;
                        }

                        if (9) {
                            {
                                {}
                                a = 5;
                            }
                        }
                }
}
