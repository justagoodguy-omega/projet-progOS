#include <stdint.h>
#include "gameboy.h"
#include "component.h"
#include "bus.h"
#include "error.h"
#include "bootrom.h"

// ======================================================================
int gameboy_create(gameboy_t* gameboy, const char* filename)
{
    M_REQUIRE_NON_NULL(gameboy);

    // WORK RAM
    M_REQUIRE_NO_ERR(component_create(gameboy -> components[0],
                MEM_SIZE(WORK_RAM)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, gameboy -> components[0],
                WORK_RAM_START, WORK_RAM_END));

    // ECHO_RAM
    M_REQUIRE_NO_ERR(component_shared(gameboy -> components[1],
                gameboy -> components[0]));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, gameboy -> components[1],
                ECHO_RAM_START, ECHO_RAM_END));

    // BOOT ROM
    M_REQUIRE_NO_ERR(bootrom_init(&(gameboy -> bootrom)));
    bootrom_plug(gameboy -> bootrom, gameboy -> bus);

    return ERR_NONE;
}

// ======================================================================
void gameboy_free(gameboy_t* gameboy)
{
    if (gameboy != NULL){
        for (int i = 0; i < GB_NB_COMPONENTS; ++i){
            if (gameboy -> components[i] != NULL){
                component_free(gameboy -> components[i]);
                gameboy -> components[i] = NULL;
            }
        }

        gameboy = NULL;
    }
}

// ======================================================================
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    M_REQUIRE_NO_ERR(bootrom_bus_listener(gameboy, gameboy -> cpu.PC));
}