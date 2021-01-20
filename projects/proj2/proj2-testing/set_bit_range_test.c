#include <stdio.h>  

void print_unsigned_binary(unsigned n);

int main()  
{  
    unsigned x = 0x10;
    unsigned y = 0x00;
    unsigned new = set_bit_range(x, y, 4, 1, 1);
    
    fprintf(stderr, "%s", "X:\n");
    print_unsigned_binary(x);
    fprintf(stderr, "%s", "\nY:\n");
    print_unsigned_binary(y);
    fprintf(stderr, "%s", "\nNEW:\n");
    print_unsigned_binary(new);
    return 0;  
} 

unsigned set_bit_range(unsigned src, unsigned dst, int num_bits, int src_pos, int dst_pos) {
    unsigned mask = ((1<<(num_bits))-1)<<(src_pos);

    if (dst_pos > src_pos) {
            return (dst & (~(mask << dst_pos))) | ((src & mask) << dst_pos);
    } else {
    	return (dst & (~(mask >> (src_pos - dst_pos)))) | ((src & mask) >> (src_pos - dst_pos));
    }
}


void print_unsigned_binary(unsigned n) {
	unsigned i = 0x8000;
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