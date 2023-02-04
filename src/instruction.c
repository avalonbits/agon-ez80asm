#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "hash.h"
#include "instruction.h"
#include "assemble.h"
#include "utils.h"
#include "globals.h"

bool none_match(operand *op) {
    return ((op->reg == R_NONE) && (op->immediate_provided == false));
}
bool cc_match(operand *op) {
    return false; // not implemented yet
}
bool ir_match(operand *op) {
    return ((op->reg >= R_IXH) && (op->reg <= R_IYL));
}
bool ixy_match(operand *op) {
    return (((op->reg == R_IX) || (op->reg == R_IY)) && !(op->indirect));
}
bool indirect_ixyd_match(operand *op) {
    return (((op->reg == R_IX) || (op->reg == R_IY)) && op->indirect);
}
bool mmn_match(operand *op) {
    return (!(op->indirect) && (op->immediate_provided));
}
bool indirect_mmn_match(operand *op) {
    return ((op->indirect) && (op->immediate_provided));
}
bool n_match(operand *op) {
    return (!(op->indirect) && (op->immediate_provided));
}
bool a_match(operand *op) {
    return(op->reg == R_A);
}
bool hl_match(operand *op) {
    return((op->reg == R_HL) && !(op->indirect));
}
bool indirect_hl_match(operand *op) {
    return((op->reg == R_HL) && (op->indirect));
}
bool rr_match(operand *op) {
    return((op->reg >= R_BC) && (op->reg <= R_HL) && !(op->indirect));
}
bool indirect_rr_match(operand *op) {
    return((op->reg >= R_BC) && (op->reg <= R_HL) && (op->indirect));
}
bool rxy_match(operand *op) {
    return(!(op->indirect) && ((op->reg == R_BC) || (op->reg == R_DE) || (op->reg == R_IX) || (op->reg == R_IY)));
}
bool sp_match(operand *op) {
    return(!(op->indirect) && (op->reg == R_SP));
}
bool indirect_sp_match(operand *op) {
    return((op->indirect) && (op->reg == R_SP));
}
bool r_match(operand *op) {
    return((op->reg >= R_A) && (op->reg <= R_L));
}
bool reg_r_match(operand *op) {
    return(op->reg == R_R);
}
bool mb_match(operand *op) {
    return(op->reg == R_MB);
}
bool i_match(operand *op) {
    return(op->reg == R_I);
}
bool b_match(operand *op) {
    return (!(op->indirect) && (op->immediate_provided));
}
void none_transform(opcodetransformtype type, operand *op) {
    return;
}
void cc_transform(opcodetransformtype type, operand *op) {
    return;
}
void ir_transform(opcodetransformtype type, operand *op) {
    if(type == TRANSFORM_IXY) {
        switch(op->reg) {
            case R_IXH:
                output.prefix1 = 0xDD;
                return;
            case R_IXL:
                output.prefix1 = 0xDD;
                output.opcode++;
                return;
            case R_IYH:
                output.prefix1 = 0xFD;
                return;
            case R_IYL:
                output.prefix1 = 0xFD;
                output.opcode++;
                return;
            default:
                break;

        }
    }
    error(message[ERROR_TRANSFORMATION]);
    return;
}
void ixy_transform(opcodetransformtype type, operand *op) {
    return;
}
void indirect_ixyd_transform(opcodetransformtype type, operand *op) {
    if(type == TRANSFORM_IXY) {
        switch(op->reg) {
            case R_IX:
                output.prefix1 = 0xDD;
                return;
            case R_IY:
                output.prefix1 = 0xFD;
                return;
            default:
                break;

        }
    }
    error(message[ERROR_TRANSFORMATION]);
    return;
}
void mmn_transform(opcodetransformtype type, operand *op) {
    return;
}
void indirect_mmn_transform(opcodetransformtype type, operand *op) {
    return;
}
void n_transform(opcodetransformtype type, operand *op) {
    return;
}
void a_transform(opcodetransformtype type, operand *op) {
    return;
}
void hl_transform(opcodetransformtype type, operand *op) {
    return;
}
void indirect_hl_transform(opcodetransformtype type, operand *op) {
    return;
}
void rr_transform(opcodetransformtype type, operand *op) {
    switch(type) {
        case TRANSFORM_P:
            output.opcode |= (op->reg_index << 4);
            return;
        default:
            error(message[ERROR_TRANSFORMATION]);
    }
    return;
}
void indirect_rr_transform(opcodetransformtype type, operand *op) {
    return;
}
void rxy_transform(opcodetransformtype type, operand *op) {
    return;
}
void sp_transform(opcodetransformtype type, operand *op) {
    return;
}
void indirect_sp_transform(opcodetransformtype type, operand *op) {
    return;
}
void r_transform(opcodetransformtype type, operand *op) {
    switch(type) {
        case TRANSFORM_Z:
            output.opcode |= op->reg_index;
            return;
        case TRANSFORM_Y:
            output.opcode |= (op->reg_index << 3);
            return;
        default:
            error(message[ERROR_TRANSFORMATION]);
    }
    return;
}
void reg_r_transform(opcodetransformtype type, operand *op) {
    return;
}
void mb_transform(opcodetransformtype type, operand *op) {
    return;
}
void i_transform(opcodetransformtype type, operand *op) {
    return;
}
void b_transform(opcodetransformtype type, operand *op) {
    switch(type) {
        case TRANSFORM_Y:
            output.opcode |= (op->immediate << 3);
            return;
        default:
            error(message[ERROR_TRANSFORMATION]);
    }
    return;
}
instruction * instruction_table[INSTRUCTION_TABLE_SIZE]; // hashtable of possible instructions, indexed by mnemonic name
operandtype_match operandtype_matchlist[] = {            // table with fast access to functions that perform matching to an specific operandtype
    {OPTYPE_NONE, none_match, none_transform},
    {OPTYPE_CC, cc_match, cc_transform},
    {OPTYPE_IR, ir_match, ir_transform},
    {OPTYPE_IXY, ixy_match, ixy_transform},
    {OPTYPE_INDIRECT_IXYd, indirect_ixyd_match, indirect_ixyd_transform},
    {OPTYPE_MMN, mmn_match, mmn_transform},
    {OPTYPE_INDIRECT_MMN, indirect_mmn_match, indirect_mmn_transform},
    {OPTYPE_N, n_match, n_transform},
    {OPTYPE_A, a_match, a_transform},
    {OPTYPE_HL, hl_match, hl_transform},
    {OPTYPE_INDIRECT_HL, indirect_hl_match, indirect_hl_transform},
    {OPTYPE_RR, rr_match, rr_transform},
    {OPTYPE_INDIRECT_RR, indirect_rr_match, indirect_rr_transform},
    {OPTYPE_RXY, rxy_match, rxy_transform},
    {OPTYPE_SP, sp_match, sp_transform},
    {OPTYPE_INDIRECT_SP, indirect_sp_match, indirect_sp_transform},
    {OPTYPE_R, r_match, r_transform},
    {OPTYPE_REG_R, reg_r_match, reg_r_transform},
    {OPTYPE_MB, mb_match, mb_transform},
    {OPTYPE_I, i_match, i_transform},
    {OPTYPE_BIT, b_match, b_transform}
};

unsigned int collisions;    // internal use

operandlist operands_adc[] = {
    {OPTYPE_A, OPTYPE_INDIRECT_HL,  TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0x8E, SL_ONLY}, // tested
    {OPTYPE_A, OPTYPE_IR,           TRANSFORM_NONE, TRANSFORM_IXY,  0xDD, 0x00, 0x8C, NONE}, // tested voorlopig, twijfel over IX/Y code
    {OPTYPE_A, OPTYPE_INDIRECT_IXYd,TRANSFORM_NONE, TRANSFORM_IXY,  0xDD, 0x00, 0x8E, SL_ONLY}, // tested voorlopig, twijfel over IX/Y code
    {OPTYPE_A, OPTYPE_N,            TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0xCE, NONE}, // tested
    {OPTYPE_A, OPTYPE_R,            TRANSFORM_NONE, TRANSFORM_Z,    0x00, 0x00, 0x88, NONE}, // tested
    {OPTYPE_HL, OPTYPE_RR,          TRANSFORM_NONE, TRANSFORM_P,    0x00, 0xED, 0x4A, SL_ONLY}, // tested
    {OPTYPE_HL, OPTYPE_SP,          TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0xED, 0x7A, SL_ONLY}, // tested
};
operandlist operands_add[] = {
    {OPTYPE_A, OPTYPE_INDIRECT_HL,  TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0x86, SL_ONLY}, // tested
    {OPTYPE_A, OPTYPE_IR,           TRANSFORM_NONE, TRANSFORM_IXY,  0xDD, 0x00, 0x84, NONE},
    {OPTYPE_A, OPTYPE_INDIRECT_IXYd,TRANSFORM_NONE, TRANSFORM_IXY,  0xDD, 0x00, 0x86, SL_ONLY},
    {OPTYPE_A, OPTYPE_N,            TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0xC6, NONE},
    {OPTYPE_A, OPTYPE_R,            TRANSFORM_NONE, TRANSFORM_Z,    0x00, 0x00, 0x80, NONE},
    {OPTYPE_A, OPTYPE_RR,           TRANSFORM_NONE, TRANSFORM_P,    0x00, 0x00, 0x09, SL_ONLY},
    {OPTYPE_A, OPTYPE_SP,           TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0x39, SL_ONLY},
    {OPTYPE_IXY, OPTYPE_RXY,        TRANSFORM_IXY, TRANSFORM_P,     0xDD, 0x00, 0x09, SL_ONLY}, // zeker testen
    {OPTYPE_IXY, OPTYPE_SP,         TRANSFORM_IXY, TRANSFORM_NONE,  0xDD, 0x00, 0x39, SL_ONLY}, // zeker testen
};
operandlist operands_and[] = {
    {OPTYPE_A, OPTYPE_INDIRECT_HL,  TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0xA6, SL_ONLY},
    {OPTYPE_A, OPTYPE_IR,           TRANSFORM_NONE, TRANSFORM_IXY,  0xDD, 0x00, 0xA4, NONE},
    {OPTYPE_A, OPTYPE_INDIRECT_IXYd,TRANSFORM_NONE, TRANSFORM_IXY,  0xDD, 0x00, 0xA6, SL_ONLY},
    {OPTYPE_A, OPTYPE_N,            TRANSFORM_NONE, TRANSFORM_NONE, 0x00, 0x00, 0xE6, NONE},
    {OPTYPE_A, OPTYPE_R,            TRANSFORM_NONE, TRANSFORM_Z,    0x00, 0x00, 0xA0, NONE},
};
operandlist operands_bit[] = {
    {OPTYPE_BIT, OPTYPE_INDIRECT_HL,  TRANSFORM_Y, TRANSFORM_NONE,  0x00, 0xCB, 0x46, SL_ONLY}, // tested
    {OPTYPE_BIT, OPTYPE_INDIRECT_IXYd,TRANSFORM_Y, TRANSFORM_IXY,   0xDD, 0xCB, 0x46, SL_ONLY}, // zeker testen!
    {OPTYPE_BIT, OPTYPE_R,            TRANSFORM_Y, TRANSFORM_Z,     0x00, 0xCB, 0x40, NONE},
};
operandlist operand_call[] = {
    {OPTYPE_CC, OPTYPE_MMN,         TRANSFORM_CC, TRANSFORM_NONE,   0x00, 0x00, 0xC4, SISLIL},

};

operandlist operands_test[] = {
    {OPTYPE_R, OPTYPE_R,            TRANSFORM_Y, TRANSFORM_Z, 0x00, 0x00, 0x80, NONE},
};
operandlist operands_ld[] = {
    {OPTYPE_R, OPTYPE_R,            TRANSFORM_Y, TRANSFORM_Z, 0x00, 0x00, 0x80, NONE}, // tested
};
instruction instructions[] = {
    {"test",EZ80, sizeof(operands_test)/sizeof(operandlist),operands_test},
    {"adc", EZ80, sizeof(operands_adc)/sizeof(operandlist), operands_adc},
    {"add", EZ80, sizeof(operands_add)/sizeof(operandlist), operands_add},
    {"and", EZ80, sizeof(operands_and)/sizeof(operandlist), operands_and},
    {"bit", EZ80, sizeof(operands_bit)/sizeof(operandlist), operands_bit},
    {"ld",  EZ80, sizeof(operands_ld)/sizeof(operandlist), operands_ld},
    {"adl", ASSEMBLER, 0, NULL}
};

/*
mnemonic mnemonics[] = {
    {.name = "adc", .function = zero_action},
    {.name = "add", .function = zero_action},
    {.name = "cp", .function = zero_action},
    {.name = "daa", .function = zero_action},
    {.name = "dec", .function = zero_action},
    {.name = "inc", .function = zero_action},
    {.name = "mlt", .function = zero_action},
    {.name = "neg", .function = zero_action},
    {.name = "sbc", .function = zero_action},
    {.name = "sub", .function = zero_action},
    {.name = "bit", .function = zero_action},
    {.name = "res", .function = zero_action},
    {.name = "set", .function = zero_action},
    {.name = "cpd", .function = zero_action},
    {.name = "cpdr", .function = zero_action},
    {.name = "cpi", .function = zero_action},
    {.name = "cpir", .function = zero_action},
    {.name = "ldd", .function = zero_action},
    {.name = "lddr", .function = zero_action},
    {.name = "ldi", .function = zero_action},
    {.name = "ldir", .function = zero_action},
    {.name = "ex", .function = zero_action},
    {.name = "exx", .function = zero_action},
    {.name = "in", .function = zero_action},
    {.name = "in0", .function = zero_action},
    {.name = "ind", .function = zero_action},
    {.name = "indr", .function = zero_action},
    {.name = "indrx", .function = zero_action},
    {.name = "ind2", .function = zero_action},
    {.name = "ind2r", .function = zero_action},
    {.name = "indm", .function = zero_action},
    {.name = "indmr", .function = zero_action},
    {.name = "ini", .function = zero_action},
    {.name = "inir", .function = zero_action},
    {.name = "inirx", .function = zero_action},
    {.name = "ini2", .function = zero_action},
    {.name = "ini2r", .function = zero_action},
    {.name = "inim", .function = zero_action},
    {.name = "inimr", .function = zero_action},
    {.name = "otdm", .function = zero_action},
    {.name = "otdmr", .function = zero_action},
    {.name = "otdrx", .function = zero_action},
    {.name = "otim", .function = zero_action},
    {.name = "otimr", .function = zero_action},
    {.name = "otirx", .function = zero_action},
    {.name = "out", .function = zero_action},
    {.name = "out0", .function = zero_action},
    {.name = "outd", .function = zero_action},
    {.name = "otdr", .function = zero_action},
    {.name = "outd2", .function = zero_action},
    {.name = "otd2r", .function = zero_action},
    {.name = "outi", .function = zero_action},
    {.name = "otir", .function = zero_action},
    {.name = "outi2", .function = zero_action},
    {.name = "oti2r", .function = zero_action},
    {.name = "tstio", .function = zero_action},
    {.name = "ld", .function = ld_action},
    {.name = "lea", .function = zero_action},
    {.name = "pea", .function = zero_action},
    {.name = "pop", .function = zero_action},
    {.name = "push", .function = zero_action},
    {.name = "and", .function = zero_action},
    {.name = "cpl", .function = zero_action},
    {.name = "or", .function = zero_action},
    {.name = "tst", .function = zero_action},
    {.name = "xor", .function = zero_action},
    {.name = "ccf", .function = zero_action},
    {.name = "di", .function = zero_action},
    {.name = "ei", .function = zero_action},
    {.name = "halt", .function = zero_action},
    {.name = "im", .function = zero_action},
    {.name = "nop", .function = nOPTYPE_action},
    {.name = "rsmix", .function = zero_action},
    {.name = "scf", .function = zero_action},
    {.name = "slp", .function = zero_action},
    {.name = "stmix", .function = zero_action},
    {.name = "call", .function = zero_action},
    {.name = "djnz", .function = zero_action},
    {.name = "jp", .function = zero_action},
    {.name = "jr", .function = zero_action},
    {.name = "ret", .function = ret_action},
    {.name = "reti", .function = zero_action},
    {.name = "retn", .function = zero_action},
    {.name = "rst", .function = zero_action},
    {.name = "rl", .function = zero_action},
    {.name = "rla", .function = zero_action},
    {.name = "rlc", .function = zero_action},
    {.name = "rlca", .function = zero_action},
    {.name = "rld", .function = zero_action},
    {.name = "rr", .function = zero_action},
    {.name = "rra", .function = zero_action},
    {.name = "rrc", .function = zero_action},
    {.name = "rrca", .function = zero_action},
    {.name = "rrd", .function = zero_action},
    {.name = "sla", .function = zero_action},
    {.name = "sra", .function = zero_action},
    {.name = "srl", .function = zero_action},
    {.name = "adl", .function = adl_action}
};
*/

bool instruction_table_insert(instruction *p){
    int index,i,try;

    if(p == NULL) return false;
    index = hash(p->name, INSTRUCTION_TABLE_SIZE);
   
    for(i = 0; i < INSTRUCTION_TABLE_SIZE; i++) {
        try = (i + index) % INSTRUCTION_TABLE_SIZE;
        if(instruction_table[try] == NULL){
            instruction_table[try] = p;
            return true;
        } 
        else collisions++;
    }
    return false;
}

void init_instruction_table(){
    int i;
    for(i = 0; i < INSTRUCTION_TABLE_SIZE; i++){
        instruction_table[i] = NULL;
    }
    collisions = 0;

    for(i = 0; i < sizeof(instructions)/sizeof(instruction);i++){
        instruction_table_insert(&instructions[i]);
    }
}

unsigned int instruction_table_entries(){
    unsigned int i,count = 0;
    for(i = 0; i < INSTRUCTION_TABLE_SIZE; i++){
        if(instruction_table[i] != NULL) count++;
    }
    return count;
}


void print_instruction_table(){
    int i;
    for(i = 0; i < INSTRUCTION_TABLE_SIZE; i++){
        if(instruction_table[i] == NULL) {
            printf("\t%i\t---\n",i);
        } else {
            printf("\t%i\t%s\n",i,instruction_table[i]->name);
        }
    }
}

instruction * instruction_table_lookup(char *name){
    int index,i,try;
    index = hash(name, INSTRUCTION_TABLE_SIZE);
    for(i = 0; i < INSTRUCTION_TABLE_SIZE; i++){
        try = (index + i) % INSTRUCTION_TABLE_SIZE;
        if(instruction_table[try] == NULL){
            return NULL;
        }
        if(instruction_table[try] != NULL &&
            strncmp(instruction_table[try]->name,name,INSTRUCTION_TABLE_SIZE) == 0){
            return instruction_table[try];
        }
    }
    return NULL;
}