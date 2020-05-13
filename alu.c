#include "bit.h"
#include "alu.h"
#include "error.h"

//=====================================================================================================================
/**
 * @brief get flag value
 *
 * @param flags flag set to get flag from
 * @param flag flag value to get
 * @return flag value
 */
flag_bit_t get_flag(flags_t flags, flag_bit_t flag)
{
    if(flag != FLAG_Z && flag != FLAG_N && flag != FLAG_H && flag != FLAG_C){
        return 0;
    } else {
        flag_bit_t res = flags & flag;
        return res;
    }
}

//=====================================================================================================================
/**
 * @brief set flag
 *
 * @param flags (modified) set of flags
 * @param flag flag to be set
 */
void set_flag(flags_t* flags, flag_bit_t flag)
{
    if (flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C){
        *flags = *flags | flag;
    }
}

//===========================================HELPER=FOR=SETTING=FLAGS==================================================
int set_flags_value(alu_output_t* result, uint16_t zero, uint8_t setH, uint8_t setC)
{
    M_REQUIRE_NON_NULL(result);

    if (zero == 0){
        set_Z(&(result -> flags));
    }
    if (setH != 0){
        set_H(&(result -> flags));
    }
    if (setC != 0){
        set_C(&(result -> flags));
    }

    return ERR_NONE;
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
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0)
{
    M_REQUIRE_NON_NULL(result);

    uint8_t v1 = lsb4(x) + lsb4(y) + c0;
    uint8_t v2 = msb4(x) + msb4(y) + msb4(v1);
    uint8_t res = merge4(v1, v2);
    (result -> value) = res;
    (result -> flags) = 0;
    M_REQUIRE_NO_ERR(set_flags_value(result, res, msb4(v1), msb4(v2)));

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
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0)
{
    M_REQUIRE_NON_NULL(result);

    uint8_t v1 = lsb4(x) - lsb4(y) - b0;
    uint8_t v2 = msb4(x) - msb4(y) - bit_get(v1, 5);
    uint8_t res = merge4(v1, v2);
    (result -> value) = res;
    (result -> flags) = 0;
    M_REQUIRE_NO_ERR(set_flags_value(result, res, bit_get(v1, 5), bit_get(v2, 5)));
    set_N(&(result -> flags));

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
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y)
{
    M_REQUIRE_NON_NULL(result);

    uint16_t v1 = lsb8(x) + lsb8(y);
    uint8_t v1_low = lsb4(x) + lsb4(y);
    uint16_t v2 = msb8(x) + msb8(y) + msb8(v1);
    uint8_t v2_low = msb4(x) + msb4(y) + msb4(v1_low);
    uint16_t res = merge8(v1, v2);
    (result -> value) = res;
    (result -> flags) = 0;
    M_REQUIRE_NO_ERR(set_flags_value(result, res, msb4(v1_low), msb4(v2_low)));

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
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y)
{
    M_REQUIRE_NON_NULL(result);
    
    uint16_t v1 = lsb8(x) + lsb8(y);
    uint8_t v1_high = lsb4(x >> 8) + lsb4(y >> 8);
    uint16_t v2 = msb8(x) + msb8(y) + msb8(v1);
    uint8_t v2_high = msb4(x >> 8) + msb4(y >> 8) + msb4(v1_high);
    uint16_t res = merge8(v1, v2);
    (result -> value) = res;
    (result -> flags) = 0;
    M_REQUIRE_NO_ERR(set_flags_value(result, res, msb4(v1_high), msb4(v2_high)));

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

int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir)
{
    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;
    uint8_t shift_result = x;

    switch (dir) {
	case LEFT:
		if(bit_get(shift_result, MSB_INDEX_8)){
            set_C(&(result -> flags));
        }
        bit_rotate(&shift_result, dir, 1);
        bit_unset(&shift_result, 0);
        break;

	case RIGHT:
		if (bit_get(shift_result, 0)){
            set_C(&(result -> flags));
        }
        bit_rotate(&shift_result, dir, 1);
        bit_unset(&shift_result, MSB_INDEX_8);
        break;

    default: 
        return ERR_BAD_PARAMETER;
    }

    if (shift_result == 0) {
        set_Z(&(result -> flags));
    }
    result -> value = shift_result;
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

int alu_shiftR_A(alu_output_t* result, uint8_t x)
{
    M_REQUIRE_NON_NULL(result);

    (result -> flags) = 0;
    uint8_t shift_result = x;
    bit_rotate(&shift_result, RIGHT, 1);
    if (get_MSB_8(x) == 1){
        bit_set(&shift_result, MSB_INDEX_8);
    }
    M_REQUIRE_NO_ERR(set_flags_value(result, shift_result, 0, bit_get(x, 0)));
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
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir)
{
    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;
    uint8_t shift_result = x;
    bit_t setC = 0;;
    
    switch (dir) {
	case LEFT:
		setC = bit_get(x, MSB_INDEX_8);
        break;

	case RIGHT:
		setC = bit_get(x, 0);
        break;

    default: 
        return ERR_BAD_PARAMETER;
    }
    
    bit_rotate(&shift_result, dir, 1);
    result -> value = shift_result;
    M_REQUIRE_NO_ERR(set_flags_value(result, x, 0, setC));

    return ERR_NONE;
}

// ===========================================================================
/**
 * @brief logic rotate with carry taken into account
 *
 * @param result alu_output_t pointer to write into
 * @param x value to rotate
 * @param dir rotation direction
 * @param flags carry flag
 * @return error code
 */
int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags)
{
    M_REQUIRE_NON_NULL(result);
    (result -> flags) = 0;
    bit_t carry_out = 0;
    uint8_t shift_result = 0;
    bit_rotate(&shift_result, dir, 1);

    switch (dir) {
	case LEFT: 
        carry_out = bit_get(x, MSB_INDEX_8);
        if (get_C(flags)){
            bit_set(&shift_result, 0);
        } else {
            bit_unset(&shift_result, 0);
        }
        break;

	case RIGHT:
		carry_out = bit_get(shift_result, MSB_INDEX_8);
        if(get_C(flags) != 0){
            bit_set(&shift_result, MSB_INDEX_8);
        } else {
            bit_unset(&shift_result, MSB_INDEX_8);
        }
        break;

     default: 
        return ERR_BAD_PARAMETER;
    }

    result -> value = shift_result;
    M_REQUIRE_NO_ERR(set_flags_value(result, shift_result, 0, carry_out));

    return ERR_NONE;
}
