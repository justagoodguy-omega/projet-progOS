#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "bit_vector.h"
#include "bit.c"

#define CHUNK_SIZE 32
#define BYTE_SIZE 8

//================================ HELPERS ================================

/**
 *@brief calculates the number of uint_32 necessary to hold all the bits of the vector
 *@param size, the number of bits of the vector
 *@return number of uint_32 necessary to hold all the bits
 */

size_t forced_size(size_t size)
{
    size_t actual_size = size / CHUNK_SIZE;
    if (size % CHUNK_SIZE != 0){
        return actual_size + 1;
    } else {
        return actual_size;
    }
}

//==================================================================================

/**
 * @brief sets the bits allocated but not used by the vector to 0
 * @param vector, the vector whose unused bits will be set to 0
 * @param size, the number of bits actually used by the vector
 */
void bit_vector_set_unnecessary_bits_to_0(bit_vector_t* vector, size_t size){
    if(vector != NULL){
        size_t index_of_uint32 = forced_size(size) - 1;
        size_t extra_bits = size % CHUNK_SIZE;
        if (extra_bits != 0){
            uint32_t mask = 0;
            for (size_t i = extra_bits; i < CHUNK_SIZE; ++i){
                mask = mask + (1 << i);
            }
            mask = ~mask;
            (vector -> content)[index_of_uint32] = (vector -> content)[index_of_uint32] & mask;
        } 
    }
}

//========================================================================================





//============================ END HELPERS ===============================================

bit_vector_t* bit_vector_create(size_t size, bit_t value) {
    if(size == 0){
        return NULL;
    }
    size_t nb_chunks = forced_size(size);
    const size_t N_MAX = (SIZE_MAX - sizeof(bit_vector_t)) / sizeof(uint32_t) + 1; 
    if (nb_chunks <= N_MAX) {
        size_t nb_extra_chunks = nb_chunks - 1;
        
        bit_vector_t* vector = malloc(sizeof(bit_vector_t) + (nb_extra_chunks) * sizeof(uint32_t));

        if(vector != NULL) {
            vector -> size = size;

            uint32_t init_value;
            if(value == 1){
                init_value = 0xFFFFFFFF;
            } else {
                init_value = 0;
            }
            
            for (size_t i = 0; i < nb_chunks; ++i){
                vector -> content[i] = init_value;
            }
            if(value == 1){
                bit_vector_set_unnecessary_bits_to_0(vector, size);
            }
        }
        return vector;
    }
    else {
        return NULL;
    } 
}

//=========================================================================================

bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv)
{
    if (pbv == NULL){
        return NULL;
    }
    bit_vector_t* copy = (bit_vector_t*) malloc(sizeof(bit_vector_t)
        + (forced_size(pbv -> size) - 1) * sizeof(uint32_t));
    if (copy != NULL){
        copy -> size = pbv -> size;
        size_t nb_chunks =  forced_size(pbv -> size);
        for (size_t i = 0; i < nb_chunks; ++i){
            copy -> content[i] = pbv -> content[i];
        }
    }
    return copy;
}

//=========================================================================================

bit_t bit_vector_get(const bit_vector_t* pbv, size_t index)
{
    if (pbv == NULL || index >= (CHUNK_SIZE * forced_size(pbv -> size))){
        return 0;
    }
    size_t chunk = index / CHUNK_SIZE;
    size_t byte_chunk = (index - CHUNK_SIZE * chunk) / (BYTE_SIZE); 
    size_t idx = index % (BYTE_SIZE);
    uint16_t half_chunk = 0;
    if (byte_chunk <= 1){
        half_chunk = pbv -> content[chunk] & 0xFFFF;
    } else {
        half_chunk = pbv -> content[chunk] >> (CHUNK_SIZE / 2);
    }
    uint8_t byte = 0;
    if (byte_chunk % 2 == 0){
        byte = lsb8(half_chunk);
    } else {
        byte = msb8(half_chunk);
    }
    return bit_get(byte, idx);
}

//=========================================================================

bit_vector_t* bit_vector_not(bit_vector_t* pbv)
{
    if (pbv == NULL){
        return NULL;
    }
    size_t nb_chunks =  forced_size(pbv -> size);
    for (size_t i = 0; i < nb_chunks; ++i){
        pbv -> content[i] = ~(pbv -> content[i]);
    }
    bit_vector_set_unnecessary_bits_to_0(pbv, pbv -> size);
    return pbv;
}

//=========================================================================

bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{
    if(pbv1 == NULL || pbv2 == NULL || pbv1 -> size != pbv2 -> size){
        return NULL;
    }
    size_t nb_chunks =  forced_size(pbv1 -> size);
    for (size_t i = 0; i < nb_chunks; ++i){
        pbv1 -> content[i] = (pbv1 -> content[i]) & (pbv2 -> content[i]);
    }
    return pbv1;
}

//=========================================================================
bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{
    if(pbv1 == NULL || pbv2 == NULL || pbv1 -> size != pbv2 -> size){
        return NULL;
    }
    size_t nb_chunks =  forced_size(pbv1 -> size);
    for (size_t i = 0; i < nb_chunks; ++i){
        pbv1 -> content[i] = (pbv1 -> content[i]) | (pbv2 -> content[i]);
    }
    return pbv1;
}

//=========================================================================
bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{
    if(pbv1 == NULL || pbv2 == NULL || pbv1 -> size != pbv2 -> size){
        return NULL;
    }
    size_t nb_chunks =  forced_size(pbv1 -> size);
    for (size_t i = 0; i < nb_chunks; ++i){
        pbv1 -> content[i] = (pbv1 -> content[i]) ^ (pbv2 -> content[i]);
    }
    return pbv1;
}

//=========================================================================
bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{
    if(pbv == NULL){
        return NULL;
    }
    bit_vector_t* extracted = bit_vector_create(size, 0);
    if (extracted != NULL){
        size_t nb_chunks = forced_size(size);
        for(size_t j = 0; j < nb_chunks; ++j){
            for(size_t i = 0; i < CHUNK_SIZE; ++i){
                size_t index_in_original_vector = j * CHUNK_SIZE + index + i;
                if( index_in_original_vector >= 0 && (index_in_original_vector < pbv -> size)){
                    extracted -> content[j] = (extracted -> content[j]) + (bit_vector_get(pbv, index_in_original_vector) << i);
                }
            }
        }
    }
    return extracted;
}


//=========================================================================
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{
    if(pbv == NULL){
        return NULL;
    }
    bit_vector_t* extracted = bit_vector_create(size, 0);
    if (extracted != NULL){
        size_t nb_chunks = forced_size(size);
        for(size_t j = 0; j < nb_chunks; ++j){
            for(size_t i = 0; i < CHUNK_SIZE; ++i){
                size_t index_in_original_vector = j * CHUNK_SIZE + index + i;
                size_t looped_index = index_in_original_vector % pbv -> size;
                if(looped_index < 0){
                    looped_index = looped_index + pbv -> size;
                }
                extracted -> content[j] = (extracted -> content[j]) + (bit_vector_get(pbv, looped_index) << i); 
            }
        }
    }
    return extracted;
}


//=========================================================================
bit_vector_t* bit_vector_shift(const bit_vector_t* pbv, int64_t shift)
{
    return bit_vector_extract_zero_ext(pbv, -shift, pbv -> size);
}

//=========================================================================
bit_vector_t* bit_vector_join(const bit_vector_t* pbv1, const bit_vector_t* pbv2, int64_t shift)
{
    if(shift < 0 || pbv1 == NULL || pbv2 == NULL || shift > (pbv1 -> size) || (pbv1 -> size != pbv2 -> size)) {
        return NULL;
    }
    bit_vector_t* vector = bit_vector_create(pbv1 -> size, 0);
    size_t shift_chunk_index = shift / CHUNK_SIZE;
    size_t shift_bit_index = shift % CHUNK_SIZE;
    
    size_t nb_chunks = forced_size(pbv1 -> size);
    for(int j  = 0; j < nb_chunks; ++j){
        if(j < shift_chunk_index){
            vector -> content[j] = pbv1 -> content[j];
        }
        else if(j > shift_chunk_index){
            vector -> content[j] = pbv2 -> content[j];
        }
        else{
            for(size_t i = 0; i < CHUNK_SIZE; ++i){
                size_t index_in_vector = j * CHUNK_SIZE + i;
                if(i < shift_bit_index){
                    vector -> content[j] = vector -> content[j] + (bit_vector_get(pbv1, index_in_vector) << i);
                }
                else{
                    vector -> content[j] = vector -> content[j] + (bit_vector_get(pbv2, index_in_vector) << i);
                }   
            }
        }
    }
    return vector;
} 

//=========================================================================
int bit_vector_print(const bit_vector_t* pbv)
{
    size_t print_counter = 0;
    if(pbv != NULL){
        bit_vector_t* copy = bit_vector_cpy(pbv);
        size_t nb_chunks = forced_size(pbv -> size);
        for (int j = 0; j < nb_chunks; ++j){
            for (int i = 0; i < CHUNK_SIZE; ++i){
                size_t index_in_vector = j * CHUNK_SIZE + i;
                if(index_in_vector < copy -> size){
                    bit_t bit_to_print = copy -> content[j] & 1;
                    printf("%d", bit_to_print);
                    print_counter++;
                    copy -> content[j] = copy -> content[j] >> 1;
                }
            } 
        }
    }
    return print_counter;
}

//=========================================================================
int bit_vector_println(const char* prefix, const bit_vector_t* pbv)
{
    size_t print_counter = 0;
    if(prefix != NULL && pbv != NULL){
        print_counter = print_counter + printf("%s", prefix);
        print_counter = print_counter + bit_vector_print(pbv);
        printf("\n");
    }
    return print_counter;
}

//=========================================================================
void bit_vector_free(bit_vector_t** pbv)
{
    free(*pbv);
    *pbv = NULL;
}



