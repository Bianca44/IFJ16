#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "builtin.h"

int main(int argc, char *argv[]) {
        if (argc != 2) {
                //fprintf(stderr, "bad arguments\n");
                //return ERROR_INTERPRET;
        }

        int internal_error = 0;

        //printf("int %d\n", read_int());
        //printf("double %g\n", read_double());
        //printf("string %s\n", read_string());

        internal_error = init_scanner(argv[1]);
        //printf("internal_scanner_error= %d\n", internal_error);

        return internal_error;
}
