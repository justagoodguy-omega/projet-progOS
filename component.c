#include "component.h"
#include "memory.h"
#include "error.h"

/**
 * @brief Creates a component given various arguments
 *
 * @param c component pointer to initialize
 * @param mem_size size of the memory of the component
 * @return error code
 */
int component_create(component_t* c, size_t mem_size){
    if(mem_size > MAX_MEM_SIZE){
        return ERR_BAD_PARAMETER;
    }
    memory_t component_memory;
    component_memory.size = mem_size
    c = &component_t (component_memory, 0, 0);

    return ERR_NONE;

/**
 * @brief Shares memory between two components
 *
 * @param c component pointer to share to
 * @param c_old component to share from
 * @return error code
 */
int component_shared(component_t* c, component_t* c_old){

    return ERR_NOT_IMPLEMENTED;
}

/**
 * @brief Destroy's a component
 *
 * @param c component pointer to destroy
 */
void component_free(component_t* c){
    M_REQUIRE_NON_NULL(c);
    c -> start = 0;
    c -> end = 0;
}