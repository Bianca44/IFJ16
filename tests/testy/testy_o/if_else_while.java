class Main{
    static int y=0;
    static void run(){
        int x = 0;
        x = z();
        ifj16.print(x+"\n");
        return ;

    }

    static int z(){
        int x = 0;
        while(y < 10)
            {y = y + 1;}
        if(false || true && true && false) 
            y = 1;
        else{
            y = 2;
            if(y)
                if(y)
                    if(y-2){
                        if(y)
                            y = 3;
                        else
                            y = 4;
                    }
                    else
                        y = 5;
                    if(y != 5)
                        y = 6;
                    else{
                        c();
                        while(y < 42){
                            x = c();
                            y = y + x;
                            
                        }
                    }
        }
        return y;

    }
    static int c(){

            return 1;

    }
}
