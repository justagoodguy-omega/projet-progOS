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
    FILE* f = NULL;
    f = fopen(filename, "r");
    if (f == NULL || feof(f) || ferror(f)){
        return ERR_IO;
    }
    data_t* memory = calloc(BANK_ROM_SIZE, sizeof(data_t));

    uint8_t value_read = 0;
    uint16_t addr_read = 0;
    size_t size = 0;
    char* test_rep = calloc(18, sizeof(char));

    while (!feof(f) && !ferror(f) && size < BANK_ROM_SIZE){
        int err_check = 0;
        //16bit address
        err_check = fscanf(f, "%d", &addr_read);
        if (err_check != 1){
            return ERR_IO;
        }
        //:
        int c = getc(f);
        if (c == EOF || c != ':'){
            return ERR_IO;
        }
        // read line (16 bytes)
        for (int i = 0; i < 16; ++i){
            err_check = fscanf(f, "%d", &value_read);
            if (err_check != 1){
                return ERR_IO;
            }
            memory[size] = value_read;
            ++size;
        }
        //ignore text representation
        err_check = fscanf(f, "%16c", &test_rep);
        if (err_check != 16){
            return ERR_IO;
        }
        c = getc(f);
        if (c != '\n'){
            if (c != EOF){
                return ERR_IO;
            }
        }
    }
    fclose(f);
    if (memory[CARTRIDGE_TYPE_ADDR] != 0){
        return ERR_NOT_IMPLEMENTED;
    }
    if (size + 1 != BANK_ROM_SIZE){
        return ERR_BAD_PARAMETER;
    }

    c -> mem -> memory = memory;
    c -> mem -> size = BANK_ROM_SIZE;
    return ERR_NONE;
}

// ======================================================================
int cartridge_init(cartridge_t* ct, const char* filename)
{
    M_REQUIRE_NO_ERR(component_create(&(ct -> c), BANK_ROM_SIZE));
    M_REQUIRE_NO_ERR(cartridge_init_from_file(&(ct -> c), filename));
    return ERR_NONE;
}

// ======================================================================
int cartridge_plug(cartridge_t* ct, bus_t bus)
{
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
        ct = NULL;
    }
}