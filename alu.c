#include "bit.h"
#include "alu.h"
#include "error.h"

/**
 * @brief get flag value
 *
 * @param flags flag set to get flag from
 * @param flag flag value to get
 * @return flag value
 */
bit_t get_flag(flags_t flags, flag_bit_t flag){
    uint8_t res = flags & flag;
    if (res == 0) return 0;
    else return 1; 
}

/**
 * @brief set flag
 *
 * @param flags (modified) set of flags
 * @param flag flag to be set
 */
void set_flag(flags_t* flags, flag_bit_t flag){
    *flags = *flags | flag;
}

/**
 * @brief adds two uint8 and writes the results and flags into an alu_output_t structure
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @param c0 carry in
 * @return error code
 */
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0){

    M_REQUIRE_NON_NULL(result);

    uint8_t v1 = lsb4(x) + lsb4(y) + c0;
    uint8_t v2 = msb4(x) + msb4(y) + msb4(v1);
    uint8_t res = merge4(v1, v2);
    (result -> result) = res;
    (result -> flags) = 0; 
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    if (msb4(v1) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (msb4(v2) != 0){
        set_flag(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}

/**
 * @brief subtract two uint8 and writes the results and flags into an alu_output_t structure
 *
 * @param result alu_output_t pointer to write into
 * @param x value to subtract from
 * @param y value to subtract
 * @param b0 initial borrow bit
 * @return error code
 */
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0){
    
    M_REQUIRE_NON_NULL(result);

    uint8_t v1 = lsb4(x) - lsb4(y) - b0;
    uint8_t v2 = msb4(x) - msb4(y) + msb4(v1);
    uint8_t res = merge4(v1, v2);
    (result -> result) = res;
    (result -> flags) = 0;
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    if (msb8(res) != 0){
        set_flags(&(result -> flags), FLAG_N);
    }
    if (msb4(v1) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (msb4(v2) != 0){
        set_flags(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}

/**
 * @brief sum two uint16 and writes the results and flags into an alu_output_t structure,
 *        the H & C flags are being placed according to the 8 lsb
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @return error code
 */
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y){

    M_REQUIRE_NON_NULL(result);

    uint16_t v1 = lsb8(x) + lsb8(y);
    uint8_t v1_low = lsb4(x) + lsb4(y);
    uint16_t v2 = msb8(x) + msb8(y) + msb8(v1);
    uint8_t v2_low = msb4(x) + msb4(y) + msb4(v1_low);
    uint16_t res = merge8(v1, v2);
    (result -> result) = res;
    (result -> flags) = 0;
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    if (msb4(v1_low) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (msb4(v2_low) != 0){
        set_flags(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}

/**
 * @brief sum two uint16 and writes the results and flags into an alu_output_t structure,
 *        the H & C flags are being placed according to the 8 msb
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @return error code
 */
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y){

    M_REQUIRE_NON_NULL(result);
    
    uint16_t v1 = lsb8(x) + lsb8(y);
    uint8_t v1_high = lsb4(x >> 8) + lsb4(y >> 8);
    uint16_t v2 = msb8(x) + msb8(y) + msb8(v1);
    uint8_t v2_high = msb4(x >> 8) + msb4(y >> 8) + msb4(v1_high);
    uint16_t res = merge8(v1, v2);
    (result -> result) = res;
    (result -> flags) = 0;
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    if (msb4(v1_high) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (msb4(v2_high) != 0){
        set_flags(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}
