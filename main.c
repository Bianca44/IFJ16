#include <stdio.h>
#include <stdlib.h>
#include "error_codes.h"
#include "scanner.c"
#include "strings.h"

int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "bad arguments\n");
                return ERROR_INTERPRET;
        }

        int internal_error = OK;

        init_scanner(argv[1]);

        /*string s;
           init_string(&s);
           set_string(&s, "boxhoho");

           char *ss = "ho";
           find(&s, ss);

           set_string(&s, "aaaxhxs");
           find(&s, "hx");

           set_string(&s, "ahoj");
           find(&s, "oj");

           set_string(&s, "ahojkatkaado");
           find(&s, "katka");
           free_string(&s); */

        printf("path: %s \n", argv[1]);
        return internal_error;
}
