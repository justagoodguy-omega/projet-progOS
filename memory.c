/**
 * @file memory.c
 * @brief Memory for Gamemu
 *
 * @author L. Rovati & P. Oliver, EPFL
 * @date 2020
 */
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
int mem_create(memory_t* mem, size_t size){
    if (size == 0 || mem == NULL){
        return ERR_BAD_PARAMETER;
    }

    M_EXIT_IF_NULL(mem = malloc(sizeof(memory_t)), sizeof(memory_t));
    M_EXIT_IF_NULL(mem -> memory = calloc(size, sizeof(data_t)), size * sizeof(data_t));

    mem -> size = size;
    return ERR_NONE;
}

/**
 * @brief Destroys memory structure
 *
 * @param mem memory structure pointer to destroy
 */
void mem_free(memory_t* mem){
    if (mem != NULL){
        if (mem -> memory != NULL){
            free(mem -> memory);
            free(mem);
        }
        mem -> size = 0;
        mem = NULL;
    }
}