#include <stdlib.h>
#include <string.h>
#include "instructions.h"
#include "interpret.h"
#include "debug.h"
#include "strings.h"
#include "builtin.h"

#define UNUSED(x) (void)(x)

constant_t * tape_ref;
constant_t * labels;
tFrameStack frame_stack;

void dispose_inst(void * inst){
    free((tInst *)(inst));
}
//INPUT
void i_rint(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    result->i = read_int();
    d_inst_name();
}
void i_rdbl(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    result->d = read_double();
    d_inst_name();
}
void i_rstr(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    result->s = read_string();
    d_inst_name();
}
//ARITHMETIC
void i_add_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i + op2->i;
    d_inst_name();
}

void i_add_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d + op2->d;
    d_inst_name();
}

void i_sub_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i - op2->i;
    d_inst_name();
}

void i_sub_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d - op2->d;
    d_inst_name();
}

void i_mul_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i * op2->i;
    d_inst_name();
}

void i_mul_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d * op2->d;
    d_inst_name();
}

void i_div_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i / op2->i;
    d_inst_name();
    //TODO delenie nulu
}

void i_div_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d / op2->d;
    d_inst_name();
}

//CONVERSIONS
void i_conv_i_to_d(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->d = (double)op1->i;
    d_inst_name();
}

void i_to_str(tVar *op1, tVar *op2, tVar *result){
    d_inst_name(); //TODO
    switch(op1->data_type){
        case INT: //_sprintf
            break;
        case DOUBLE:
            break;
        case BOOLEAN:
            break;
        case STRING:
            break;
        default:
            fprintf(stderr, "CHYBA PRI KONVERZII STRINGU");
    }
}

//ASSIGNS
void i_assign_i(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->i = op1->i;
    d_inst_name();
}

void i_assign_d(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->d = op1->d;
    d_inst_name();
}

void i_assign_b(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->b = op1->b;
    d_inst_name();
}

void i_assign_s(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    if(op1 != result){
        if(result->initialized)
            free(result->s);
        result->s = copy_string(op1->s);
    }
    d_inst_name();
}

//first parameter instruction tape
//op1 - bool
//op2 - label

void i_goto(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    d_inst_name();

    DLSetActive(processed_tape, (tDLElemPtr)result->s);

}

void i_jnt(tVar *op1, tVar *op2, tVar *result){

    d_inst_name();
    UNUSED(op2);

    if(!op1->b){
        DLSetActive(processed_tape, (tDLElemPtr)result->s);
        d_print("%p",(void *)result);
    }

}

void i_jt(tVar *op1, tVar *op2, tVar *result){

    d_inst_name();
    UNUSED(op2);

    if(op1->b){
        DLSetActive(processed_tape, (tDLElemPtr)result->s);
        d_print("%p", (void *) result);
    }

}

//LOGICAL
//equal
void i_e_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (op1->i == op2->i);
    d_inst_name();
}

void i_e_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (op1->d == op2->d);
    d_inst_name();
}
void i_e_b(tVar *op1, tVar *op2, tVar *result){
    result->b = (op1->b == op2->b);
    d_inst_name();
}
//not equal
void i_ne_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i != op2->i);
    d_inst_name();
}

void i_ne_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d != op2->d);
    d_inst_name();
}
void i_ne_b(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->b != op2->b);
    d_inst_name();
}

//less
void i_l_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i < op2->i);
    d_inst_name();
}

void i_l_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d < op2->d);
    d_inst_name();
}

//greater
void i_g_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i > op2->i);
    d_inst_name();
}

void i_g_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d > op2->d);
    d_inst_name();
}

//less equal
void i_le_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i <= op2->i);
    d_inst_name();
}

void i_le_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d <= op2->d);
    d_inst_name();
}

//greater equal
void i_ge_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i >= op2->i);
    d_inst_name();
}

void i_ge_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d >= op2->d);
    d_inst_name();
}

//not
void i_not(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->b = !op1->b;
    d_inst_name();
}

//FUNCTIONS
//initialization of frame
//op1 size of frame

int decode_type(char type){
    switch (type) {
    case 'i':
        return INT;
    case 'd':
        return DOUBLE;
    case 'b':
        return BOOLEAN;
    case 's':
        return STRING;
    default:
       d_message("CHYBA V DECODE_TYPE");
       return -42;
    }
}

void i_init_frame(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    UNUSED(result);
    symbol_table_item_t *fun = (symbol_table_item_t *)(op1->s);
    frame_stack.prepared = init_frame(fun->function.params_local_vars_count);
    int i = fun->function.params_count;

    for(char *s = fun->function.local_vars_data_types; *s != '\0'; s++){
        frame_stack.prepared->local[i].data_type = decode_type(*s);
        frame_stack.prepared->local[i].initialized = false;
        i++;
    }

    d_inst_name();
}
//push
void i_push_param(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    UNUSED(result);
    frame_stack.prepared->local[push_counter] = *op1;
    if(op1->data_type == STRING){
        frame_stack.prepared->local[push_counter].s = copy_string(op1->s);
    }
    d_tVarPtr(op1);
    push_counter++;
    d_inst_name();
}
//
void i_f_call(tVar *op1, tVar *op2, tVar *result){
   //ulozit nasledujucu instrukciu na vrchol zasobniku
    d_message("VSTUP do funkcie");
    //setting push counter to zero
    push_counter = 0;
    UNUSED(op2);
    d_inst_name();
    //setting frame to stack top
    push_frame(&frame_stack, frame_stack.prepared);
    d_message("ramec pridany na vrchol");

    //remembering state of tape in the time of function call
    tDLElemPtr pi = DLActiveElem(processed_tape);
    tDLList *parent_tape = processed_tape;
    //change of awareness of instruction tape (jumps)
    processed_tape = (tDLList *)(op1->s);
    d_message("zapocatie tac");
    //executing function
    interpret_tac(processed_tape);
    d_message("opustenie tac");
    //realoading previous state
    processed_tape = parent_tape;
    DLSetActive(processed_tape, pi);
    //saving result
    if(result != NULL){
        switch(result->data_type){
            case INT:
                result->i = frame_stack.top->frame->ret_val->i;
                d_print("%d ==VYSL== ", result->i);
                break;
            case DOUBLE:
                result->d = frame_stack.top->frame->ret_val->d;
                d_print("%g ==VYSL== ", result->d);
                break;
            case STRING:
                free(result->s); //uvolnovat ? //TODO
                result->s =  copy_string(frame_stack.top->frame->ret_val->s);
                d_print("%s ==VYSL== ", result->s);
                break;
            case BOOLEAN:
                result->b = frame_stack.top->frame->ret_val->b;
                d_print("%d ==VYSL== ", result->b);
                break;
            default:
                fprintf(stderr, " CHYBA V NAVRATOVEJ HODNOTE");
        }

    }
    //removing frame
    pop_frame(&frame_stack);
    d_message("VYSTUP z funkcie");

}

void i_return(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    UNUSED(result);
    d_inst_name();

    frame_stack.top->frame->ret_val = op1;
    DLLast(processed_tape);
}

//BUILT-IN

void i_cat(tVar *op1, tVar *op2, tVar *result){
    d_inst_name();
}

void i_strcmp(tVar *op1, tVar *op2, tVar *result){
    d_inst_name();
    result->i = compare(op1->s, op2->s);
}

void i_substr(tVar *op1, tVar *op2, tVar *result); //TODO

void i_find(tVar *op1, tVar *op2, tVar *result){
    d_inst_name();
    result->i = find(op1->s, op2->s);
}

void i_sort(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    d_inst_name();

    if(op1 != result){
        if(result->initialized)
            free(result->s);
        result->s = copy_string(op1->s);
    }

    sort(result->s);
}

void i_print(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    UNUSED(result);
    d_inst_name();

    print(op1); // priamo sem alebo makro TODO
}

void i_len(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->i = strlen(op1->s);
    d_inst_name();
}
//GENERATING INSTRUCTIONS

void set_label(tDLElemPtr jump, tDLElemPtr where){
    ((tInst *)(jump->data))->result = insert_special_const(&labels, (void *)where);
}

tInst * generate(tInstId instruction, void *op1, void *op2, void *result){

    tInst * new_inst;

    if((new_inst = malloc(sizeof(tInst))) == NULL){
        //TODO
    }

    new_inst->op1 = op1;
    new_inst->op2 = op2;
    new_inst->result = result;
    new_inst->f = NULL;

    switch(instruction){
         //INPUT
        case I_RINT:
            new_inst->f = i_rint;
			break;
        case I_RDBL:
            new_inst->f = i_rdbl;
			break;
        case I_RSTR:
            new_inst->f = i_rstr;
			break;
        //ARITHMETIC
        case I_ADD:
            if(new_inst->result->data_type == INT)
                new_inst->f = i_add_i;
            else
                new_inst->f = i_add_d;
			break;
        case I_MUL:
            if(new_inst->result->data_type == INT)
                new_inst->f = i_mul_i;
            else
                new_inst->f = i_mul_d;
			break;
        case I_SUB:
            if(new_inst->result->data_type == INT)
                new_inst->f = i_sub_i;
            else
                new_inst->f = i_sub_d;
			break;
        case I_DIV:
            if(new_inst->result->data_type == INT)
                new_inst->f = i_div_i;
            else
                new_inst->f = i_div_d;
			break;
        //CONVERSIONS
        case I_CONV_I_TO_D:
                new_inst->f = i_conv_i_to_d;
			break;
        case I_TO_STRING:
                new_inst->f = i_to_str;
			break;
        //LOGICAL
        case I_E:
            switch(new_inst->op1->data_type){
                case INT:
                    new_inst->f = i_e_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_e_d;
                    break;
                case BOOLEAN:
                    new_inst->f = i_e_b;
                    break;
            }
			break;
        case I_NE:
            switch(new_inst->op1->data_type){
                case INT:
                    new_inst->f = i_ne_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_ne_d;
                    break;
                case BOOLEAN:
                    new_inst->f = i_ne_b;
                    break;
            }
			break;
        case I_L:
            switch(new_inst->op1->data_type){
                case INT:
                    new_inst->f = i_l_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_l_d;
                    break;
            }
			break;
        case I_G:
            switch(new_inst->op1->data_type){
                case INT:
                    new_inst->f = i_g_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_g_d;
                    break;
            }
			break;
        case I_LE:
            switch(new_inst->op1->data_type){
                case INT:
                    new_inst->f = i_le_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_le_d;
                    break;
            }
			break;
        case I_GE:
            switch(new_inst->op1->data_type){
                case INT:
                    new_inst->f = i_ge_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_ge_d;
                    break;
            }
			break;
        case I_NOT:
                new_inst->f = i_not;
			break;
        //BUILT-IN AND OTHER
        case I_ASSIGN:
            switch(new_inst->result->data_type){
                case INT:
                    new_inst->f = i_assign_i;
                    break;
                case DOUBLE:
                    new_inst->f = i_assign_d;
                    break;
                case BOOLEAN:
                    new_inst->f = i_assign_b;
                    break;
                case STRING:
                    new_inst->f = i_assign_s;
                    break;
            }
			break;
        case I_CAT:
			break;
        case I_STRCMP:
            new_inst->f = i_strcmp;
			break;
        case I_SUBSTR:
			break;
        case I_FIND:
            new_inst->f = i_find;
			break;
        case I_SORT:
            new_inst->f = i_sort;
			break;
        case I_PRINT:
            new_inst->f = i_print;
			break;
        case I_LEN:
            new_inst->f = i_len;
			break;
        //ASSOCIATED WITH FUNCTIONS
        case I_INIT_FRAME:
            //conv to tVar due to unified access
            new_inst->op1 = insert_special_const(&tape_ref, op1);
            new_inst->f = i_init_frame;
			break;
        case I_PUSH_PARAM:
            new_inst->f = i_push_param;
			break;
        case I_F_CALL:
            //conv to tVAR due to unified access
            new_inst->op1 = insert_special_const(&tape_ref, op1);
            new_inst->f = i_f_call;
			break;
        case I_RETURN:
            new_inst->f = i_return;
			break;
        //JUMPS
        case I_GOTO:
            new_inst->f = i_goto;
			break;
        case I_JNT:
            new_inst->f = i_jnt;
			break;
        case I_JT:
            new_inst->f = i_jt;
			break;
        default:
            new_inst->f = NULL;
    }

    return new_inst;
}
