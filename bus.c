#include "bus.h"
#include "error.h"
#include <stdint.h>
#include "memory.h"

int bus_remap(bus_t bus, component_t*c, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    if((c -> end) - (c -> start) + offset >= MAX_MEM_SIZE){
        return ERR_BAD_PARAMETER;
    } else {
        int visible_size = end - start;
        for (int i = 0; i <= end; ++i){
            bus[i] = c -> mem -> 
        }
        bus[start] =
        c -> start = &(c -> mem -> memory[offset]);
        c -> end = &(c -> mem -> memory[offset + visible_size]);
    }
}
