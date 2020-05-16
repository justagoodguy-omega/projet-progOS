#include <stdio.h>
#include <stdint.h>
#include "bootrom.h"
#include "component.h"
#include "error.h"

// ======================================================================
int bootrom_init(component_t* c)
{
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NO_ERR(component_create(c, MEM_SIZE(BOOT_ROM)));
    data_t content[MEM_SIZE(BOOT_ROM)] = GAMEBOY_BOOT_ROM_CONTENT;
    for (size_t i = 0; i < MEM_SIZE(BOOT_ROM); ++i){
        *(c -> mem -> memory + i) = content[i];
    }
    
    return ERR_NONE;
}

// ======================================================================
int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr)
{
    M_REQUIRE_NON_NULL(gameboy);
    if (gameboy -> boot){
        if (addr == REG_BOOT_ROM_DISABLE){
            M_REQUIRE_NO_ERR(bus_unplug(gameboy -> bus, &(gameboy -> bootrom)));
            M_REQUIRE_NO_ERR(cartridge_plug(&(gameboy -> cartridge), gameboy -> bus));
            gameboy -> boot = 0;
        }
    }

    return ERR_NONE;
}