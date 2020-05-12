#include <stdio.h>
#include <stdint.h>   // for uint8_t and uint16_t types
#include <inttypes.h> // for PRIx8, etc.
#include "bit.h"
#include "error.h"

// ======================================================================
#define size(T) (sizeof T / sizeof T[0])
#define FMT8 "0x%02" PRIx8
#define FMT16 "0x%04" PRIx16
#define show(M, S1) printf(#M "(" FMT ## S1 ") = " FMT8 "\n", value, result)
#define show2(M, V) printf("bit_" #M "(" FMT8 ", %d) = " FMT8 "\n", V, k, value2)



// ======================================================================
/**
 * @brief returns the 4 LSB of a uint8_t
 *
 * @param value value to get the 4 LSB from
 * @return 4 LSB of the input
 */
uint8_t lsb4(uint8_t value)
{
    return value & 0xf;
}

// ======================================================================
/**
 * @brief returns the 4 MSB of a uint8_t
 *
 * @param value value to get the 4 MSB from
 * @return 4 MSB of the input
 */
uint8_t msb4(uint8_t value)
{
    return value >> 4;
}

// ======================================================================
/**
 * @brief returns the 8 LSB of a uint16_t
 *
 * @param value value to get the 8 LSB from
 * @return 8 LSB of the input
 */
uint8_t lsb8(uint16_t value)
{
    return value & 0xff;
}

// ======================================================================
/**
 * @brief returns the 8 MSB of a bit16_t (uint16_t)
 *
 * @param value value to get the 8 MSB from
 * @return 8 MSB of the input
 */
uint8_t msb8(uint16_t value)
{
    return value >> 8;
}

// ======================================================================
/**
 * @brief Merges two 8bit into 16bits
 *
 * @param v1 value for the 8 LSB
 * @param v2 value for the 8 MSB
 * @return result of merging v1 and v2 into 16bits
 */
uint16_t merge8(uint8_t v1, uint8_t v2)
{
    return ((v2 & 0xff) << 8) | (v1 & 0xff);
}

// ======================================================================
/**
 * @brief Merges two 4bit into 8bits
 *
 * @param v1 value for the 4 LSB
 * @param v2 value for the 4 MSB
 * @return result of merging v1 and v2 into 8bits
 */
uint8_t merge4(uint8_t v1, uint8_t v2)
{
    return ((v2 & 0xf) << 4) | (v1 & 0xf);
}

// ======================================================================
/**
 * @brief Gets the bit at a given index
 *
 * @param value value to get the bit from
 * @param index index of the bit
 * @return returns the bit at a given index
 */
bit_t bit_get(uint8_t value, int index)
{
    return ((value & (1 << CLAMP07(index))) == 0) ? 0 : 1;
}

// ======================================================================
/**
 * @brief Set the bit at a given index to 1
 *
 * @param value value to change the bit in
 * @param index index of the bit
 */
void bit_set(uint8_t* value, int index)
{
    M_REQUIRE_NON_NULL(value);
    *value =  *value | (1 << CLAMP07(index));
}

// ======================================================================
/**
 * @brief Set the bit at a given index to 0
 *
 * @param value value to change the bit in
 * @param index index of the bit
 */
void bit_unset(uint8_t* value, int index)
{
    M_REQUIRE_NON_NULL(value);
    *value = *value & ~(1 << CLAMP07(index));
}

// ======================================================================
/**
 * @brief Rotates the bits in a uint8_t
 *
 * @param value pointer to the number to rotate
 * @param dir rotation direction
 * @param d number of rotation steps to execute
 */
void bit_rotate(uint8_t* value, rot_dir_t dir, int d)
{
    M_REQUIRE_NON_NULL(value);
    uint8_t res = *value;
    int index = CLAMP07(d);
    switch (dir) {
	case LEFT:
		res = res >> (8 - index);
		*value = *value << index;
		*value = *value | res;
		break;
	case RIGHT:
		res = res >> index;
		*value = *value << (8 - index);
		*value = *value | res;
		break;
    }
}

// ======================================================================
/**
 * @brief Set/Unset the bit at a given index
 *
 * @param value value to change the bit in
 * @param index index of the bit
 * @param v allows to choose between set and unset
 */
void bit_edit(uint8_t* value, int index, uint8_t v)
{
    M_REQUIRE_NON_NULL(value);
    if (v == 0){
        bit_unset(value, index);
    } else {
        bit_set(value, index); 
    }
}

//=====================================================================
/**
* @brief returns the msb of value
* @param value the value of which we want msb
*/
bit_t get_MSB_8(uint8_t value)
{
    return bit_get(value, MSB_INDEX_8);
}
