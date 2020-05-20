#include <stddef.h>
#include <stdint.h>
#include "bit_vector.h"
#include "bit.c"

#define CHUNK_SIZE 32
#define BYTE_SIZE 8

//================================ HELPERS ================================
size_t forced_size(size_t size)
{
    size_t actual_size = size / 32;
    if (size % 32 != 0){
        return actual_size + 1;
    } else {
        return actual_size;
    }
}

//=========================================================================
bit_vector_t* bit_vector_create(size_t size, bit_t value)
{
    size_t nb_chunks = forced_size(size);
    bit_vector_t* vector = (bit_vector_t*) malloc(sizeof(bit_vector_t)
        + (nb_chunks - 1) * sizeof(uint32_t));
    uint32_t init_value = 0;
    if (value){
        init_value = -1;
    }
    if (vector != NULL){
        vector -> nb_chunks = nb_chunks;
        for (size_t i = 0; i < nb_chunks; ++i){
            vector -> chunks[i] = init_value;
        }
    }
    return vector;
}

//=========================================================================
bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv)
{
    if (pbv == NULL){
        return NULL;
    }
    bit_vector_t* copy = (bit_vector_t*) malloc(sizeof(bit_vector_t)
        + (pbv -> nb_chunks - 1) * sizeof(uint32_t));
    if (copy != NULL){
        copy -> nb_chunks = pbv -> nb_chunks;
        for (size_t i = 0; i < pbv -> nb_chunks; ++i){
            copy -> chunks[i] = pbv -> chunks[i];
        }
    }
    return copy;
}

//=========================================================================
bit_t bit_vector_get(const bit_vector_t* pbv, size_t index)
{
    if (pbv == NULL || index >= (CHUNK_SIZE * pbv -> nb_chunks)){
        return 0;
    }
    size_t chunk = index / CHUNK_SIZE;
    size_t byte_chunk = (index - CHUNK_SIZE * chunk) / (BYTE_SIZE); 
    size_t idx = index % (BYTE_SIZE);
    uint16_t half_chunk = 0;
    if (byte_chunk <= 1){
        half_chunk = pbv -> chunks[chunk] & 0xFFFF;
    } else {
        half_chunk = pbv -> chunks[chunk] >> (CHUNK_SIZE / 2);
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
    uint32_t temp = 0;
    for (size_t i = 0; i < pbv -> nb_chunks; ++i){
        temp = pbv -> chunks[i];
        pbv -> chunks[i] = ~(pbv -> chunks[i]);
        // assez pour controler reussite de l'operation ?
        if (temp & pbv -> chunks[i] != 0){
            return NULL;
        }
    }
    return pbv;
}

//=========================================================================
bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{

}

//=========================================================================
bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{

}

//=========================================================================
bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{

}

//=========================================================================
bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{

}

//=========================================================================
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{

}

//=========================================================================
bit_vector_t* bit_vector_shift(const bit_vector_t* pbv, int64_t shift)
{

}

//=========================================================================
bit_vector_t* bit_vector_join(const bit_vector_t* pbv1, const bit_vector_t* pbv2, int64_t shift)
{

}

//=========================================================================
int bit_vector_print(const bit_vector_t* pbv)
{

}

//=========================================================================
int bit_vector_println(const char* prefix, const bit_vector_t* pbv)
{

}

//=========================================================================
void bit_vector_free(bit_vector_t** pbv)
{

}
