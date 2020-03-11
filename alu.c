#include "bit.h"
#include "alu.h"

/**
 * @brief adds two uint8 with a provided carry
 *
 * @param x value to sum
 * @param y value to sum
 * @param c0 carry in
 * @return addition result
 */
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0){
    uint8_t v1 = lsb4(x) + lsb4(y) + c0;
    uint8_t v2 = msb4(x) + msb4(y) + msb4(v1);
    uint8_t res = merge4(v1, v2);
    (result -> flags) = 0; 
    if (res == 0){
        set_flag(result -> flags, FLAG_Z);
    }
    if (msb4(v1) != 0){
        set_flag(result -> flags, FLAG_H);
    }
    
}

/**
 * @brief subtract two uint8 (x - y), substracting a provided carry (b0, ``borrow bit'')
 *
 * @param x value to subtract from
 * @param y value to subtract
 * @param b0 initial borrow bit
 * @return substraction result
 */
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0){
    /**
    uint8_t v1 = lsb4(x) - lsb4(y) - b0;
    uint8_t v2 = msb4(x) - msb4(y) + msb4(v1);
    return merge4(v1, v2);*/
}

bit_t get_flag(flags_t flags, flag_bit_t flag){
    uint8_t res = flags & flag;
    if (res == 0) return 0;
    else return 1; 
}

void set_flag(flags_t* flags, flag_bit_t flag){
    *flags = *flags | flag;
}

