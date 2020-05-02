#include <stdio.h>
#include <stdint.h>
#include "bootrom.h"
#include "component.h"
#include "error.h"

// ======================================================================
int bootrom_init(component_t* c)
{
    M_REQUIRE_NO_ERR(component_create(c, MEM_SIZE(BOOT_ROM)));
    c -> mem -> memory = GAMEBOY_BOOT_ROM_CONTENT;
    return ERR_NONE;
}

// ======================================================================
int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr)
{
    if (gameboy -> boot){
        M_REQUIRE_NO_ERR(bus_unplug(gameboy -> bus, &(gameboy -> bootrom)));
        cartridge_plug();
        gameboy -> bootrom = 0;
    }

    return ERR_NONE;
}