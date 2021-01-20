#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

unsigned get_bit_range(unsigned, unsigned, unsigned);
unsigned set_bit_range(int, int, int, int, int);
void print_unsigned_binary(unsigned);

void debug_handle_invalid_instruction(Instruction instruction);

/* Sign extends the given field to a 32-bit integer where field is
 * interpreted an n-bit integer. */ 
int sign_extend_number(unsigned int field, unsigned int n) {
    unsigned int most_significant_bit = (field >> (n - 1)) & 1;
    int result = 0;
    result = result + field;
    int mask;
    if (most_significant_bit) { /* Most significant bit is 1. */
        mask = -1 - ((1 << n) - 1); /* All 1's in extra left spaces. */
        result = result | mask; /* Apply the mask (fill in empty bits with most significant bit) */
    } else { /* Most significant bit is 0. */
        mask = 0 + ((1 << n) - 1); /* All 0's in extra left spaces, 1's in used spaces. */
        result = result & mask; /* Apply the mask (fill in empty bits with most significant bit) */
    }
    
    return result;
}

/* Unpacks the 32-bit machine code instruction given into the correct
 * type within the instruction struct */ 
Instruction parse_instruction(uint32_t instruction_bits) {
    Instruction instruction;
    
    unsigned opcode = instruction_bits & ((1 << 7) - 1); /* Extract last 7 bits */

    switch(opcode) {
        case 0x33:
            /* R-Type */
            instruction.rtype.opcode = get_bit_range(instruction_bits, 0, 6);
            instruction.rtype.rd = get_bit_range(instruction_bits, 7, 11);
            instruction.rtype.funct3 = get_bit_range(instruction_bits, 12, 14);
            instruction.rtype.rs1 = get_bit_range(instruction_bits, 15, 19);
            instruction.rtype.rs2 = get_bit_range(instruction_bits, 20, 24);
            instruction.rtype.funct7 = get_bit_range(instruction_bits, 25, 31);

            break;
        case 0x13: case 0x3: case 0x73:
            /* I-Type */
            instruction.itype.opcode = get_bit_range(instruction_bits, 0, 6);
            instruction.itype.rd = get_bit_range(instruction_bits, 7, 11);
            instruction.itype.funct3 = get_bit_range(instruction_bits, 12, 14);
            instruction.itype.rs1 = get_bit_range(instruction_bits, 15, 19);
            instruction.itype.imm = get_bit_range(instruction_bits, 20, 31);
            break;
        case 0x23:
            /* S-Type */
            instruction.stype.opcode = get_bit_range(instruction_bits, 0, 6);
            instruction.stype.imm5 = get_bit_range(instruction_bits, 7, 11);
            instruction.stype.funct3 = get_bit_range(instruction_bits, 12, 14);
            instruction.stype.rs1 = get_bit_range(instruction_bits, 15, 19);
            instruction.stype.rs2 = get_bit_range(instruction_bits, 20, 24);
            instruction.stype.imm7 = get_bit_range(instruction_bits, 25, 31);
            break;
        case 0x63:
            /* SB-Type */
            instruction.sbtype.opcode = get_bit_range(instruction_bits, 0, 6);
            /* FIX */
            instruction.sbtype.imm5 = get_bit_range(instruction_bits, 7, 11);
            instruction.sbtype.funct3 = get_bit_range(instruction_bits, 12, 14);
            instruction.sbtype.rs1 = get_bit_range(instruction_bits, 15, 19);
            instruction.sbtype.rs2 = get_bit_range(instruction_bits, 20, 24);
            /* FIX */
            instruction.sbtype.imm7 = get_bit_range(instruction_bits, 25, 31);
            break;
        case 0x37:
            /* U-Type (LUI) */
            instruction.utype.opcode = get_bit_range(instruction_bits, 0, 6);
            instruction.utype.rd = get_bit_range(instruction_bits, 7, 11);
            instruction.utype.imm = get_bit_range(instruction_bits, 12, 31);
            break;
        case 0x6F:
            /* UJ-Type */
            instruction.ujtype.opcode = get_bit_range(instruction_bits, 0, 6);
            instruction.ujtype.rd = get_bit_range(instruction_bits, 7, 11);
            /* FIX */
            instruction.ujtype.imm = get_bit_range(instruction_bits, 12, 31);
            break;
        default: // undefined opcode
            fprintf(stderr, "%s %d", "Undefined opcode in instruction: ", instruction_bits);
            break;
    }
    
    return instruction;
}

/* Return the number of bytes (from the current PC) to the branch label using the given
 * branch instruction */
int get_branch_offset(Instruction instruction) {
    unsigned imm5 = instruction.sbtype.imm5;
    unsigned imm7 = instruction.sbtype.imm7;

    unsigned imm_actual = 0;
    imm_actual = set_bit_range(imm7, imm_actual, 1, 6, 12);
    imm_actual = set_bit_range(imm7, imm_actual, 6, 0, 5);
    imm_actual = set_bit_range(imm5, imm_actual, 4, 1, 1);
    imm_actual = set_bit_range(imm5, imm_actual, 1, 0, 11);

    /*fprintf(stderr, "%s", "IMM7: ");
    print_unsigned_binary(imm7);
    fprintf(stderr, "%s", ", IMM5: ");
    print_unsigned_binary(imm5);
    fprintf(stderr, "%s", "\n");
    
    fprintf(stderr, "%s", "BEFORE: ");
    print_unsigned_binary(imm_actual);
    fprintf(stderr, "%s", "\n");*/

    int imm_actual_int = sign_extend_number(imm_actual, 12);

    /*fprintf(stderr, "%s", "AFTER: ");
    print_unsigned_binary(imm_actual_int);
    fprintf(stderr, "%s", "\n");*/

    //imm_actual_int = imm_actual_int << 1;

    /*fprintf(stderr, "%s", "AFTER MULTIPLY: ");
    print_unsigned_binary(imm_actual_int);
    fprintf(stderr, "%s", "\n");*/

    return imm_actual_int; 
}

/* Returns the number of bytes (from the current PC) to the jump label using the given
 * jump instruction */
int get_jump_offset(Instruction instruction) {
    unsigned imm = instruction.ujtype.imm;

    /*fprintf(stderr, "%s", "BEFORE: ");
    print_unsigned_binary(imm);
    fprintf(stderr, "%s", "\n");*/

    unsigned imm_actual = 0;
    imm_actual = set_bit_range(imm, imm_actual, 1, 19, 20);
    /*fprintf(stderr, "%s", "1.) ");
    print_unsigned_binary(imm_actual);
    fprintf(stderr, "%s", "\n");*/
    imm_actual = set_bit_range(imm, imm_actual, 10, 9, 1);
    /*fprintf(stderr, "%s", "2.) ");
    print_unsigned_binary(imm_actual);
    fprintf(stderr, "%s", "\n");*/
    imm_actual = set_bit_range(imm, imm_actual, 1, 8, 11);
    /*fprintf(stderr, "%s", "3.) ");
    print_unsigned_binary(imm_actual);
    fprintf(stderr, "%s", "\n");*/
    imm_actual = set_bit_range(imm, imm_actual, 8, 0, 12);
    /*fprintf(stderr, "%s", "4.) ");
    print_unsigned_binary(imm_actual);
    fprintf(stderr, "%s", "\n");*/

    int imm_actual_int = sign_extend_number(imm_actual, 20);

    //imm_actual_int = imm_actual_int << 1;

    return imm_actual_int;
}

int get_store_offset(Instruction instruction) {
    unsigned imm5 = instruction.stype.imm5;
    unsigned imm7 = instruction.stype.imm7;

    unsigned imm_actual = 0;
    imm_actual = set_bit_range(imm7, imm_actual, 7, 0, 5);
    imm_actual = set_bit_range(imm5, imm_actual, 5, 0, 0);

    int imm_actual_int = sign_extend_number(imm_actual, 12);

    //imm_actual_int = imm_actual_int << 1;

    return imm_actual_int;
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits); 
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}

void debug_handle_invalid_instruction(Instruction instruction) {
    fprintf(stderr, "%s 0x%08x\n", "DEBUGGING PRINT: Invalid Instruction: ", instruction.bits); 
}


unsigned get_bit_range(unsigned input, unsigned lower, unsigned upper) {
    return (input >> lower) & ~(~0 << (upper - lower + 1));
    /* Right shift off the unused lower (right) bits, then
        mask off the unused upper bits by anding with all 
        1s in the wanted bits and 0s in the unwanted left
        bits. */ 
}

unsigned set_bit(unsigned input, unsigned pos, unsigned val) {
    input ^= (-val ^ input) & (1 << pos);
    return input;
}

/*
unsigned set_bit_range(unsigned src, unsigned dst, int num_bits, int src_pos, int dst_pos) {
    unsigned mask = ((1<<(num_bits))-1)<<(src_pos);

    if (dst_pos > src_pos) {
            return (dst & (~(mask << dst_pos))) | ((src & mask) << dst_pos);
    } else {
        return (dst & (~(mask >> (src_pos - dst_pos)))) | ((src & mask) >> (src_pos - dst_pos));
    }
}  
*/  

unsigned set_bit_range(int src, int dst, int num_bits, int src_pos, int dst_pos) {
    int mask = ((1 << num_bits) - 1) << src_pos;
    if (dst_pos > src_pos) {
        return ((src & mask) << (dst_pos - src_pos)) | (dst & ~(mask << (dst_pos - src_pos)));
    } else {
        return ((src & mask) >> (src_pos - dst_pos)) | (dst & ~(mask >> (src_pos - dst_pos)));
    }
}   

void print_unsigned_binary(unsigned n) {
    unsigned i = 0x80000;
    while (i != 0) {
        if (n & i) {
            fprintf(stderr, "%s", "1");
        } else {
            fprintf(stderr, "%s", "0");
        }
        i >>= 1;
    }
    return;
}                      

