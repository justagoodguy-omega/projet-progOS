#include "bus.h"
#include "error.h"
#include <stdint.h>
#include "memory.h"
#include "bit.h"

int bus_remap(bus_t bus, component_t*c, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    if(((c -> end) - (c -> start) + offset >= MAX_MEM_SIZE) || (c -> start >  c-> end) || (c -> end > 0xFFFF)){
        return ERR_ADDRESS;
    } else {
        for (int i = c -> start; i <= c -> end; ++i){
            bus[i] = &(c -> mem.memory[offset + (i - c -> start)]); 
        }
    }
    return ERR_NONE;
}

int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    c -> start = start;
    c -> end = end;
    if(bus_remap(bus, c, offset) == ERR_ADDRESS){
        c -> start = 0;
        c -> end = 0;
        return ERR_ADDRESS;
    }
    return ERR_NONE;
    
}

int bus_plug(bus_t bus, component_t*c, addr_t start, addr_t end){
    M_REQUIRE_NON_NULL(c);
    if(start > end || start > 0xFFFF || end > 0xFFFF){
        return ERR_ADDRESS;
    }
    for(int i = start; i <= end; ++i){
        if(bus[i] != NULL){
            return ERR_ADDRESS;
        }
    }
    return bus_forced_plug(bus, c, start, end, 0);

}

int bus_unplug(bus_t bus, component_t* c){
    M_REQUIRE_NON_NULL(c);
    for(int i = c -> start; i <= c -> end; ++i){
        bus[i] = NULL;
    }
    c -> start = 0;
    c -> end = 0;
    return ERR_NONE;
}

int bus_read(const bus_t bus, addr_t address, data_t* data){
    M_REQUIRE_NON_NULL(data);
    if(address > 0xFFFF){
        return ERR_ADDRESS;
    }
    if(bus[address] == NULL){
        *data = 0xFF;
        return ERR_NONE;
    } else {
        *data = *bus[address];
        return ERR_NONE;
    }
}

int bus_read16(const bus_t bus, addr_t address, addr_t* data16){
    M_REQUIRE_NON_NULL(data16);
    if(address >= 0xFFFF){
        return ERR_ADDRESS;
    }
    if(bus[address] == NULL || bus[address + 1] == NULL){
        *data16 = 0xFF;
        return ERR_NONE;
    } else {
        data_t v1 = *bus[address];
        data_t v2 = *bus[address + 1];
        *data16 = merge8(v1, v2);
        return ERR_NONE;
    }
}

int bus_write(bus_t bus, addr_t address, data_t data){
    M_REQUIRE_NON_NULL(bus[address]);
    if(address > 0xFFFF){
        return ERR_ADDRESS;
    } else {
        bus[address] = &data;
        return ERR_NONE; 
    }
}

int bus_write16(bus_t bus, addr_t address, addr_t data16){
     M_REQUIRE_NON_NULL(bus[address]);
    if(address >= 0xFFFF){
        return ERR_ADDRESS;
    }
    data_t v1 = lsb8(data16);
    data_t v2 = msb8(data16);
    *bus[address] = v1;
    *bus[address + 1] = v2;
    return ERR_NONE;
}



