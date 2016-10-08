#include <stdio.h>
#include <stdlib.h>
#include "scanner.c"
#include "strings.h"


int main(int argc, char *argv[]) {
        if (argc != 2) {
                //fprintf(stderr, "bad arguments\n");
                //return ERROR_INTERPRET;
        }

        int internal_error = 0;

        internal_error = init_scanner(argv[1]);
        printf("internal_scanner_error= %d\n", internal_error);

        return internal_error;
}
