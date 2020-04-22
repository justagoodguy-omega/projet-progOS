#include <stdlib.h>
#include "memory.h"
#include "error.h"

/**
 * @brief Creates memory structure
 *
 * @param mem memory structure pointer to initialize
 * @param size size of the memory to create
 * @return error code
 */
int mem_create(memory_t* mem, size_t size)
{
    M_REQUIRE_NON_NULL(mem);
    if (size == 0){
        return ERR_BAD_PARAMETER;
    }

    M_EXIT_IF_NULL(mem -> memory = calloc(size, sizeof(data_t)), size * sizeof(data_t));

    mem -> size = size;
    return ERR_NONE;
}

/**
 * @brief Destroys memory structure
 *
 * @param mem memory structure pointer to destroy
 */
void mem_free(memory_t* mem)
{
    if (mem != NULL){
        if (mem -> memory != NULL){
            free(mem -> memory);
            mem -> memory = NULL;
        }
        mem -> size = 0;
        mem = NULL;
    }
}