#include "component.h"
#include "memory.h"
#include "error.h"


int component_create(component_t* c, size_t mem_size){
    if(mem_size > MAX_MEM_SIZE){
        return ERR_BAD_PARAMETER;
    }
    memory_t component_memory;
    component_memory.size = mem_size;
    component_t c_struct = {component_memory, 0, 0};
    c = &c_struct;

    return ERR_NONE;
}

void component_free(component_t* c){
    if(c != NULL){
        c -> start = 0;
        c -> end = 0;
    }
}