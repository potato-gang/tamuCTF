#include <stdio.h>
#include <stdint.h>

int main (void) {
    union {
        uint32_t bitstring;
        float as_float;
    } val;
    
    val.bitstring = 0b11000010100011000111111111101110;
    printf("%f ", val.as_float);
    
    val.bitstring = 0b11000001100101000011101111011111;
    printf("%f\n", val.as_float);
}
