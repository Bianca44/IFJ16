#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "list.h"
#include "error_codes.h"
#include "interpret.h"
#include "memory_constants.h"
#include "instructions.h"
#include "interpret.h"
#include "builtin.h"

extern constant_t *mem_constants;

int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Wrong arguments. Run as: ./main file.\n");
                return INTERNAL_INTERPRET_ERROR;
        }

        FILE *source_file = init_scanner(argv[1]);
        if (source_file == NULL) {
                fprintf(stderr, "Wrong path, file does not exists.\n");
                return INTERNAL_INTERPRET_ERROR;
        }

        atexit(cleanup_resources);

        tList inst_tape;
        InitList(&inst_tape, dispose_inst);



        if (parse(&inst_tape) == SYNTACTIC_ANALYSIS_ERROR) {
                fprintf(stderr, "Syntactic analysis failed.\n");
                exit(SYNTACTIC_ANALYSIS_ERROR);
        }


        parse(&inst_tape);

        symbol_table_item_t *run_method = get_symbol_table_class_item("Main", "run");

        tList *run_tape = run_method->function.instruction_tape;
        InsertLast(&inst_tape, generate(I_INIT_FRAME, run_method, NULL, NULL));
        InsertLast(&inst_tape, generate(I_F_CALL, run_tape, NULL, NULL));

        processed_tape = &inst_tape;
        init_frame_stack(&frame_stack);
        interpret_tac(&inst_tape);

        exit(INTERPRET_OK);
}
