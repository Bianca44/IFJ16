#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "DLList.h"
#include "error_codes.h"
#include "interpret.h"
#include "memory_constants.h"
#include "instructions.h"
#include "interpret.h"

extern constant_t * mem_constants;

int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Wrong arguments for interpret.\n");
                return INTERNAL_INTERPRET_ERROR;
        }

        // atexit dma

        FILE *file;

        file = init_scanner(argv[1]);
        if (file == NULL) {
            fprintf(stderr, "Wrong path to file.\n");
            return INTERNAL_INTERPRET_ERROR;
        }

        tDLList inst_tape;
        DLInitList(&inst_tape, NULL /*todo */);
        // x = 2
        void * x = insert_string_const(&mem_constants, "Hello World");
        DLInsertLast(&inst_tape, generate(I_PRINT, x, NULL, NULL));

        printf("\x1b[32mFIRST PASS\x1b[0m\n");
        if (parse(&inst_tape) == SYNTACTIC_ANALYSIS_ERROR) {
                fprintf(stderr, "Syntactic analysis failed.\n");
                return SYNTACTIC_ANALYSIS_ERROR;
        }

        printf("SYNTACTIC ANALYSIS\tOK\n");
        printf("\x1b[32mSECOND PASS\x1b[0m\n");
        parse(&inst_tape);

        printf("INTERPRET:\n");
        //interpret_tac(&inst_tape);

        //DLDisposeList(&inst_tape);

        return INTERPRET_OK;
}
