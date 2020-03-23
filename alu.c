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
flag_bit_t get_flag(flags_t flags, flag_bit_t flag){
    if(flag != FLAG_Z && flag != FLAG_N && flag != FLAG_H && flag != FLAG_C){
        return 0;
    } else {
        flag_bit_t res = flags & flag;
        return res;
    }
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

//=====================================================================================================================
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
    (result -> value) = res;
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

//==============================================================================================================
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
    (result -> value) = res;
    (result -> flags) = 0;
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    set_flag(&(result -> flags), FLAG_N);

    if (msb4(v1) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (x < y + b0){
        set_flag(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}

//===========================================================================================================
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
    (result -> value) = res;
    (result -> flags) = 0;
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    if (msb4(v1_low) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (msb4(v2_low) != 0){
        set_flag(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}

//===========================================================================================================
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
    (result -> value) = res;
    (result -> flags) = 0;
    if (res == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    if (msb4(v1_high) != 0){
        set_flag(&(result -> flags), FLAG_H);
    }
    if (msb4(v2_high) != 0){
        set_flag(&(result -> flags), FLAG_C);
    }

    return ERR_NONE;
}

// ===========================================================================
/**
 * @brief logical shift by 1 in direction of dir. The fanions are Z00C
 * 
 * @param result The value we will store the result and flags of the operation 
 * @param x the uint that will be shifted
 * @param dir the direction of the shift, LEFT or RIGHT
 */

int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir){

    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;

    switch (dir) {
	case LEFT:
		if(bit_get(x, MSB_INDEX_8) == 1){
            set_flag(&(result -> flags), FLAG_C);
        }
        x = x << 1;

        if (x == 0) {
            set_flag(&(result -> flags), FLAG_Z);
        }
        result -> value = x;
        break;

	case RIGHT:
		if (bit_get(x, 0) == 1){
            set_flag(&(result -> flags), FLAG_C);
        }
        x = x >> 1;
        if (x == 0) {
            set_flag(&(result -> flags), FLAG_Z);
        }
        result -> value = x;
        break;

    default: 
        return ERR_BAD_PARAMETER;
    }
    return ERR_NONE;
}

// ===========================================================================
/**
 * @brief arithmetic shift by 1 to the right (incoming bit may not be 0).
 * The fanions are Z00C. C corresponds to the ejected bit
 * 
 * @param result The value we will store the result and flags of the operation 
 * @param x the uint that will be shifted
 */

int alu_shiftR_A(alu_output_t* result, uint8_t x){

    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;
    uint8_t shift_result = x >> 1;

    if (bit_get(x, 0) == 1){
        set_flag(&(result -> flags), FLAG_C);
    }
    if (get_MSB_8(x) == 1){
        bit_set(&shift_result, MSB_INDEX_8);
    }
    if( shift_result == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    result -> value = shift_result;
    return ERR_NONE;
    
}

// ======================================================================
/**
 * @brief Rotate the bits of x by 1 spot in direction dir and store result 
 * and flags in result. The fanions are Z00C
 * 
 * @param result The value we will store the result and flags of the operation 
 * @param x uint that will be rotated
 * @param dir the direction of the rotation, LEFT or RIGHT
 */
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir){
    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;
    switch (dir) {
	case LEFT:
		if (bit_get(x, MSB_INDEX_8) == 1) {
            set_flag(&(result -> flags), FLAG_C);
        }
        break;

	case RIGHT:
		if (bit_get(x, 0) == 1){
            set_flag(&(result -> flags), FLAG_C);
        }
        break;

    default: 
        return ERR_BAD_PARAMETER;
    }
    bit_rotate(&x, dir, 1);
    result -> value = x;
    if (x == 0 ){
        set_flag(&(result -> flags), FLAG_Z);
    }
    return ERR_NONE;
}

// ===========================================================================
/**
 * @brief simulate a 9 bit rotation left or right, where the 9th bit (msb) is 
 * the bit from flag C. The fanions are Z00C
 * 
 * @param result The value we will store the result and flags of the operation 
 * @param x uint that will be rotated
 * @param dir the direction of the rotation, LEFT or RIGHT
 * 
 */

int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags){
    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;
    bit_t msb_x = 0;
    bit_t lsb_x = 0;
    switch (dir) {
	case LEFT: 
		msb_x = get_MSB_8(x);
        x = x << 1;
        if(get_C(flags) != 0){
            x = x + 1;
        }
        if (msb_x == 1){
            set_flag(&(result -> flags), FLAG_C);
        }
        break;

	case RIGHT:
		lsb_x = bit_get(x, 0);
        x = x >> 1;
        if(get_C(flags) != 0){
            x = x + (1 << MSB_INDEX_8);
        }
        if (lsb_x == 1){
            set_flag(&(result -> flags), FLAG_C);
        }
        break;

     default: 
        return ERR_BAD_PARAMETER;
    }
    if(x == 0){
        set_flag(&(result -> flags), FLAG_Z);
    }
    result -> value = x;
    return ERR_NONE;
}
