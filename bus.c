/**
 * @file memory.c
 * @brief Memory for Gamemu
 *
 * @author P. Oliver & L. Rovati, EPFL
 * @date 2020
 */
#include "bus.h"
#include "error.h"
#include <stdint.h>
#include "memory.h"
#include "bit.h"

/**
 * @brief Plug a component into the bus
 *
 * @param bus bus to plug into
 * @param c component to plug into bus
 * @param start address from where to plug (included)
 * @param end address until where to plug (included)
 * @return error code
 */
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

/**
 * @brief Plug forcibly a component into the bus (allows to use outside of range start and end).
 *        For example, if we want to map a component to somewhere else than the initialy described area.
 *
 * @param bus bus to plug into
 * @param c component to plug into bus
 * @param start address from where to plug to (included)
 * @param end address until where to plug to (included)
 * @param offset offset where to start in the component
 * @return error code
 */
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

/**
 * @brief Remap the memory of a component to the bus
 *
 * @param bus bus to remap to
 * @param c component to remap
 * @param offset new offset to use
 * @return error code
 */
int bus_remap(bus_t bus, component_t* c, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c -> mem -> memory); // ### Necessaire?
    if(((c -> end) - (c -> start) + offset >= c -> mem -> size) || (c -> start >  c-> end) || (c -> end > 0xFFFF)){
        return ERR_ADDRESS;
    }
    
    for (int i = c -> start; i <= c -> end; ++i){
        bus[i] = &(c -> mem -> memory[offset + (i - c -> start)]); 
    }

    return ERR_NONE;
}

/**
 * @brief Unplug a component from the bus
 *
 * @param bus bus to unplug from
 * @param c component to plug into bus
 * @return error code
 */
int bus_unplug(bus_t bus, component_t* c){
    M_REQUIRE_NON_NULL(c);
    for(int i = c -> start; i <= c -> end; ++i){
        bus[i] = NULL;
    }
    c -> start = 0;
    c -> end = 0;
    return ERR_NONE;
}

/**
 * @brief Read the bus at a given address
 *
 * @param bus bus to read from
 * @param address address to read at
 * @param data pointer to write read data to
 * @return error code
 */
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

/**
 * @brief Write to the bus at a given address
 *
 * @param bus bus to write to
 * @param address address to write at
 * @param data data to write
 * @return error code
 */
int bus_write(bus_t bus, addr_t address, data_t data){
    M_REQUIRE_NON_NULL(bus[address]);
    if(address > 0xFFFF){
        return ERR_ADDRESS;
    } else {
        bus[address] = &data;
        return ERR_NONE; 
    }
}

/**
 * @brief Read the bus at a given address (reads 16 bits)
 *
 * @param bus bus to read from
 * @param address address to read at
 * @param data16 pointer to write read data to
 * @return error code
 */
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

/**
 * @brief Write to the bus at a given address (writes 16 bits)
 *
 * @param bus bus to write to
 * @param address address to write at
 * @param data16 data to write
 * @return error code
 */
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


