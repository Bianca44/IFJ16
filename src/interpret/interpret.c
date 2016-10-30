#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "datatypes.h"

int interpret_tac(tDLList *inst_tape){
    
    DLFirst(inst_tape);
    tInst * inst;
    while(DLActive(inst_tape)){
        
        DLCopy(inst_tape, (void **)&inst);
        
        inst->f(inst->op1, inst->op2, inst->result);
        
        DLSucc(inst_tape);  
    } 
    
    
   return 0; //TODO
}
