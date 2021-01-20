#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* WARNING: DO NOT CHANGE THIS FILE. 
 YOU SHOULD DEFINITELY CONSULT IT THOUGH... */

/* convenient(?) data definitions */
typedef uint8_t  Byte; /* unsigned, 8-bit */
typedef uint16_t Half; /* unsigned, 16-bit */
typedef uint32_t Word; /* unsigned, 32-bit */
typedef uint64_t Double; /* unsigned, 64-bit */

typedef int8_t  sByte; /* signed, 8-bit */
typedef int16_t sHalf; /* signed, 16-bit */
typedef int32_t sWord; /* signed, 32-bit */
typedef int64_t sDouble; /* signed, 64-bit */

/* A memory address */
typedef Word Address; /* unsigned, 32-bit */

/* A register value */
typedef Word Register; /* unsigned 32-bit*/

/* The processor data: 
    32 registers
    LO & HI special registers
    PC program counter */
typedef struct {
    Register R[32];
    Register PC;
} Processor;

/* Possible lengths of data, and their lengths in bytes.
 These are used to align memory*/
typedef enum {
    LENGTH_BYTE = 1,
    LENGTH_HALF_WORD = 2,
    LENGTH_WORD = 4,
} Alignment;

/* This is the length of the memory space */
#define MEMORY_SPACE (1024*1024) /* 1 MByte of Memory */

/* If you haven't seen a union before, go look it up.
   Seriously. They're fun. */
typedef union {
   
    /* access opcode with: instruction.opcode */
    struct {
    unsigned int opcode : 7;
        unsigned int rest : 25;
    };
    
    /* access rtype with: instruction.rtype.(opcode|rd|funct3|rs1|rs2|funct7) */
    struct {
    unsigned int opcode : 7;
    unsigned int rd : 5;
    unsigned int funct3 : 3;
    unsigned int rs1 : 5;
    unsigned int rs2 : 5;
    unsigned int funct7 : 7;
    } rtype;
    
    /* access itype with: instruction.itype.(oppcode|rs|rt|imm) */
    struct {
    unsigned int opcode : 7;
    unsigned int rd : 5;    
    unsigned int funct3 : 3;
    unsigned int rs1 : 5;
    unsigned int imm : 12;
    } itype;
    
    /* access jtype with: instruction.jtype.(opcode|addr) */
    struct {
    unsigned int opcode : 7;
    unsigned int rd : 5;
    unsigned int imm : 20;
    } utype;

    struct {
    unsigned int opcode : 7;
    unsigned int rd : 5;
    unsigned int imm : 20;
    } ujtype;

    struct {
    unsigned int opcode : 7;
    unsigned int imm5: 5;
    unsigned int funct3: 3;
    unsigned int rs1 : 5;
    unsigned int rs2 : 5;
    unsigned int imm7 : 7;
    } stype;

     struct {
    unsigned int opcode : 7;
    unsigned int imm5: 5;
    unsigned int funct3: 3;
    unsigned int rs1 : 5;
    unsigned int rs2 : 5;
    unsigned int imm7 : 7;
    } sbtype;

   
    /* basically ignore this stuff*/
    int16_t chunks16[2];
    uint32_t bits;
} Instruction;



#endif
#define RTYPE_FORMAT "%s\tx%d, x%d, x%d\n"
#define ITYPE_FORMAT "%s\tx%d, x%d, %d\n"
#define MEM_FORMAT "%s\tx%d, %d(x%d)\n"
#define LUI_FORMAT "lui\tx%d, %d\n"
#define JAL_FORMAT "jal\tx%d, %d\n"
#define BRANCH_FORMAT "%s\tx%d, x%d, %d\n"
#define ECALL_FORMAT "ecall\n"

int sign_extend_number(unsigned, unsigned);
Instruction parse_instruction(uint32_t);
int get_branch_offset(Instruction);
int get_jump_offset(Instruction);
int get_store_offset(Instruction);
void handle_invalid_instruction(Instruction);
void handle_invalid_read(Address);
void handle_invalid_write(Address);

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

void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void print_lui(Instruction);
void print_jal(Instruction);
void print_ecall(Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction); 
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);

void debug_print_rtype(char *, Instruction);
void debug_print_itype_except_load(char *, Instruction, int);
void debug_print_load(char *, Instruction);
void debug_print_store(char *, Instruction);
void debug_print_branch(char *, Instruction);
void debug_print_lui(Instruction);
void debug_print_jal(Instruction);
void debug_print_ecall(Instruction);
void debug_write_rtype(Instruction);
void debug_write_itype_except_load(Instruction); 
void debug_write_load(Instruction);
void debug_write_store(Instruction);
void debug_write_branch(Instruction);
void print_debug_instruction(uint32_t instruction_bits);
void debug_handle_invalid_instruction(Instruction instruction);

void decode_instruction(uint32_t instruction_bits);



int main() {
    decode_instruction(0x00c000ef);
    decode_instruction(0xfff00413);
    decode_instruction(0x00008067);
    decode_instruction(0xfff00513);
    decode_instruction(0xfff00413);
    decode_instruction(0x0080006f);
    decode_instruction(0x00008067);
    decode_instruction(0xfff00513);
}


void decode_instruction(uint32_t instruction_bits) {
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            write_rtype(instruction);
            break;
        case 0x13:
            write_itype_except_load(instruction);
            break;
        case 0x3:
            write_load(instruction);
            break;
        case 0x23:
            write_store(instruction);
            break;
        case 0x63:
            write_branch(instruction);
            break;
        case 0x37:
            print_lui(instruction);
            break;
        case 0x6F:
            print_jal(instruction);
            break;
        case 0x73:
            print_ecall(instruction);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_rtype(Instruction instruction) {
    switch (instruction.rtype.funct3) {
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    print_rtype("add", instruction);
                    break;
                case 0x1:
                    print_rtype("mul", instruction);
                    break;
                case 0x20:
                    print_rtype("sub", instruction);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                break;      
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                print_rtype("sll", instruction);
                break;
                case 0x1:
                print_rtype("mulh", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x2:
            print_rtype("slt", instruction);
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:   
                print_rtype("xor", instruction);
                break;
                case 0x1:
                print_rtype("div", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                print_rtype("srl", instruction);
                break;
                case 0x20:
                print_rtype("sra", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                print_rtype("or", instruction);
                break;
                case 0x1:
                print_rtype("rem", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x7:
            print_rtype("and", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
        break;
    }
}

void write_itype_except_load(Instruction instruction) {
    int shiftOp;
    switch (instruction.itype.funct3) {
        case 0x0:
            print_itype_except_load("addi", instruction, instruction.itype.imm);
            break;
        case 0x1:
            print_itype_except_load("slli", instruction, instruction.itype.imm);
            break;
        case 0x2:
            print_itype_except_load("slti", instruction, instruction.itype.imm);
            break;
        case 0x4:
            print_itype_except_load("xori", instruction, instruction.itype.imm);
            break;
        case 0x5:
            shiftOp = instruction.itype.imm >> 10;
            switch(shiftOp) {
                case 0x0:
                    print_itype_except_load("srli", instruction, instruction.itype.imm & 0x1F);
                    break;
                case 0x1:
                    print_itype_except_load("srai", instruction, instruction.itype.imm & 0x1F);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        case 0x6:
            print_itype_except_load("ori", instruction, instruction.itype.imm);
            break;
        case 0x7:
            print_itype_except_load("andi", instruction, instruction.itype.imm);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;  
    }
}

void write_load(Instruction instruction) {
    switch (instruction.itype.funct3) {
        case 0x0:
            print_load("lb", instruction);
            break;
        case 0x1:
            print_load("lh", instruction);
            break;
        case 0x2:
            print_load("lw", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_store(Instruction instruction) {
    switch (instruction.stype.funct3) {
        case 0x0:
            print_store("sb", instruction);
            break;
        case 0x1:
            print_store("sh", instruction);
            break;
        case 0x2:
            print_store("sw", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_branch(Instruction instruction) {
    switch (instruction.sbtype.funct3) {
        case 0x0:
            print_branch("beq", instruction);
            break;
        case 0x1:
            print_branch("bne", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void print_lui(Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
    fprintf(stderr, LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
}

void print_jal(Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, JAL_FORMAT, instruction.ujtype.rd, get_jump_offset(instruction));
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, JAL_FORMAT, instruction.ujtype.rd, get_jump_offset(instruction));
    fprintf(stderr, JAL_FORMAT, instruction.ujtype.rd, get_jump_offset(instruction));
}

void print_ecall(Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, ECALL_FORMAT);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, ECALL_FORMAT);
    fprintf(stderr, ECALL_FORMAT);
}

void print_rtype(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
    fprintf(stderr, RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
}

void print_itype_except_load(char *name, Instruction instruction, int imm) {
    unsigned imm_raw = (unsigned) imm;
    int imm_ext = sign_extend_number(imm_raw, 12);
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, imm_ext);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, imm_ext);
    fprintf(stderr, ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, imm_ext);
}

void print_load(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, MEM_FORMAT, name, instruction.itype.rd, instruction.itype.imm, instruction.itype.rs1);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, MEM_FORMAT, name, instruction.itype.rd, instruction.itype.imm, instruction.itype.rs1);
    fprintf(stderr, MEM_FORMAT, name, instruction.itype.rd, instruction.itype.imm, instruction.itype.rs1);
}

void print_store(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, MEM_FORMAT, name, instruction.stype.rs2, get_store_offset(instruction), instruction.stype.rs1);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, MEM_FORMAT, name, instruction.stype.rs2, get_store_offset(instruction), instruction.stype.rs1);
    fprintf(stderr, MEM_FORMAT, name, instruction.stype.rs2, get_store_offset(instruction), instruction.stype.rs1);
}

void print_branch(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, BRANCH_FORMAT, name, instruction.sbtype.rs1, instruction.sbtype.rs2, get_branch_offset(instruction));
    fprintf(stderr, "%s", "\n");*/
    fprintf(stdout, BRANCH_FORMAT, name, instruction.sbtype.rs1, instruction.sbtype.rs2, get_branch_offset(instruction));
    fprintf(stderr, BRANCH_FORMAT, name, instruction.sbtype.rs1, instruction.sbtype.rs2, get_branch_offset(instruction));
}

void print_debug_instruction(uint32_t instruction_bits) {
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            debug_write_rtype(instruction);
            break;
        case 0x13:
            debug_write_itype_except_load(instruction);
            break;
        case 0x3:
            debug_write_load(instruction);
            break;
        case 0x23:
            debug_write_store(instruction);
            break;
        case 0x63:
            debug_write_branch(instruction);
            break;
        case 0x37:
            debug_print_lui(instruction);
            break;
        case 0x6F:
            debug_print_jal(instruction);
            break;
        case 0x73:
            debug_print_ecall(instruction);
            break;
        default: // undefined opcode
            debug_handle_invalid_instruction(instruction);
            break;
    }
}

void debug_write_rtype(Instruction instruction) {
    switch (instruction.rtype.funct3) {
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    debug_print_rtype("add", instruction);
                    break;
                case 0x1:
                    debug_print_rtype("mul", instruction);
                    break;
                case 0x20:
                    debug_print_rtype("sub", instruction);
                    break;
                default:
                    debug_handle_invalid_instruction(instruction);
                break;      
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                debug_print_rtype("sll", instruction);
                break;
                case 0x1:
                debug_print_rtype("mulh", instruction);
                break;
                default:
                debug_handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x2:
            debug_print_rtype("slt", instruction);
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:   
                debug_print_rtype("xor", instruction);
                break;
                case 0x1:
                debug_print_rtype("div", instruction);
                break;
                default:
                debug_handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                debug_print_rtype("srl", instruction);
                break;
                case 0x20:
                debug_print_rtype("sra", instruction);
                break;
                default:
                debug_handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                debug_print_rtype("or", instruction);
                break;
                case 0x1:
                debug_print_rtype("rem", instruction);
                break;
                default:
                debug_handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x7:
            debug_print_rtype("and", instruction);
            break;
        default:
            debug_handle_invalid_instruction(instruction);
        break;
    }
}

void debug_write_itype_except_load(Instruction instruction) {
    int shiftOp;
    switch (instruction.itype.funct3) {
        case 0x0:
            debug_print_itype_except_load("addi", instruction, instruction.itype.imm);
            break;
        case 0x1:
            debug_print_itype_except_load("slli", instruction, instruction.itype.imm);
            break;
        case 0x2:
            debug_print_itype_except_load("slti", instruction, instruction.itype.imm);
            break;
        case 0x4:
            debug_print_itype_except_load("xori", instruction, instruction.itype.imm);
            break;
        case 0x5:
            shiftOp = instruction.itype.imm >> 10;
            switch(shiftOp) {
                case 0x0:
                    debug_print_itype_except_load("srli", instruction, instruction.itype.imm & 0x1F);
                    break;
                case 0x1:
                    debug_print_itype_except_load("srai", instruction, instruction.itype.imm & 0x1F);
                    break;
                default:
                    debug_handle_invalid_instruction(instruction);
                    break;
            }
            break;
        case 0x6:
            debug_print_itype_except_load("ori", instruction, instruction.itype.imm);
            break;
        case 0x7:
            debug_print_itype_except_load("andi", instruction, instruction.itype.imm);
            break;
        default:
            debug_handle_invalid_instruction(instruction);
            break;  
    }
}

void debug_write_load(Instruction instruction) {
    switch (instruction.itype.funct3) {
        case 0x0:
            debug_print_load("lb", instruction);
            break;
        case 0x1:
            debug_print_load("lh", instruction);
            break;
        case 0x2:
            debug_print_load("lw", instruction);
            break;
        default:
            debug_handle_invalid_instruction(instruction);
            break;
    }
}

void debug_write_store(Instruction instruction) {
    switch (instruction.stype.funct3) {
        case 0x0:
            debug_print_store("sb", instruction);
            break;
        case 0x1:
            debug_print_store("sh", instruction);
            break;
        case 0x2:
            debug_print_store("sw", instruction);
            break;
        default:
            debug_handle_invalid_instruction(instruction);
            break;
    }
}

void debug_write_branch(Instruction instruction) {
    switch (instruction.sbtype.funct3) {
        case 0x0:
            debug_print_branch("beq", instruction);
            break;
        case 0x1:
            debug_print_branch("bne", instruction);
            break;
        default:
            debug_handle_invalid_instruction(instruction);
            break;
    }
}

void debug_print_lui(Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
}

void debug_print_jal(Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, JAL_FORMAT, instruction.ujtype.rd, get_jump_offset(instruction));
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, JAL_FORMAT, instruction.ujtype.rd, get_jump_offset(instruction));
}

void debug_print_ecall(Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, ECALL_FORMAT);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, ECALL_FORMAT);
}

void debug_print_rtype(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
}

void debug_print_itype_except_load(char *name, Instruction instruction, int imm) {
    unsigned imm_raw = (unsigned) imm;
    int imm_ext = sign_extend_number(imm_raw, 12);
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, imm_ext);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, imm_ext);
}

void debug_print_load(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, MEM_FORMAT, name, instruction.itype.rd, instruction.itype.imm, instruction.itype.rs1);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, MEM_FORMAT, name, instruction.itype.rd, instruction.itype.imm, instruction.itype.rs1);
}

void debug_print_store(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, MEM_FORMAT, name, instruction.stype.rs2, get_store_offset(instruction), instruction.stype.rs1);
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, MEM_FORMAT, name, instruction.stype.rs2, get_store_offset(instruction), instruction.stype.rs1);
}

void debug_print_branch(char *name, Instruction instruction) {
    /*fprintf(stderr, "%s", "\nMY OUTPUT: ");
    fprintf(stderr, BRANCH_FORMAT, name, instruction.sbtype.rs1, instruction.sbtype.rs2, get_branch_offset(instruction));
    fprintf(stderr, "%s", "\n");*/
    fprintf(stderr, BRANCH_FORMAT, name, instruction.sbtype.rs1, instruction.sbtype.rs2, get_branch_offset(instruction));
}



 
