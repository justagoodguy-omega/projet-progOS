#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "component.h"
#include "bus.h"
#include "error.h"
#include "memory.h"
#include "cartridge.h"

// ======================================================================
int cartridge_init_from_file(component_t* c, const char* filename)
{
    M_REQUIRE_NON_NULL(c);
    if (filename == NULL){
        return ERR_BAD_PARAMETER;
    } else {
        if (strlen(filename) == 0){
            return ERR_BAD_PARAMETER;
        }
    }
    FILE* f = NULL;
    f = fopen(filename, "rb");
    if (f == NULL){
        return ERR_IO;
    } else {
        if(feof(f) || ferror(f)){
            fclose(f);
            return ERR_IO;  
        }
    }
    
    size_t err = 0;
    err = fread(c -> mem -> memory, 1, BANK_ROM_SIZE, f);
    if (err != BANK_ROM_SIZE){
        fclose(f);
        return ERR_IO;
    }

    fclose(f);
    if ((c -> mem -> memory)[CARTRIDGE_TYPE_ADDR] != 0){
        return ERR_NOT_IMPLEMENTED;
    }

    return ERR_NONE;
}

// ======================================================================
int cartridge_init(cartridge_t* ct, const char* filename)
{
    M_REQUIRE_NON_NULL(ct);
    if (filename == NULL){
        return ERR_BAD_PARAMETER;
    } else {
        if (strlen(filename) == 0){
            return ERR_BAD_PARAMETER;
        }
    }
    component_t c;
    memset(&c, 0, sizeof(component_t));
    M_REQUIRE_NON_NULL(&c);
    M_REQUIRE_NO_ERR(component_create(&c, BANK_ROM_SIZE));
    M_REQUIRE_NO_ERR(cartridge_init_from_file(&c, filename));
    ct -> c = c;
    return ERR_NONE;
}

// ======================================================================
int cartridge_plug(cartridge_t* ct, bus_t bus)
{
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NO_ERR(bus_forced_plug(bus, &(ct -> c), BANK_ROM0_START, BANK_ROM1_END, 0));
    return ERR_NONE;
}

// ======================================================================
void cartridge_free(cartridge_t* ct)
{
    if (ct != NULL) {
        if (&(ct -> c) != NULL){
            component_free(&(ct -> c));
        }
    }
    ct = NULL;
}