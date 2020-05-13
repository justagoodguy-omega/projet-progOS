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

    /*### CORR ###*/
    //init components?
    component_t* comps = calloc(GB_NB_COMPONENTS, sizeof(component_t));
    M_REQUIRE_NON_NULL(comps);
    cpu_t* cpu = (cpu_t*)malloc(sizeof(cpu_t));
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NO_ERR(cpu_init(cpu));
    bus_t bus; //how to init??

    gameboy -> bus = bus;
    gameboy -> components = comps;
    gameboy -> cpu = *cpu;
    /*### END CORR ###*/

    // WORK RAM
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[0]),
                MEM_SIZE(WORK_RAM)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[0]),
                WORK_RAM_START, WORK_RAM_END));

    // ECHO_RAM
    /*### CORR ###*/
    // what to do with a temp component?? Stock in gameboy?
    component_t* echo_ram = (component_t*)malloc(sizeof(component_t));
    M_REQUIRE_NO_ERR(component_shared(echo_ram, &(gameboy -> components[0])));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, echo_ram, ECHO_RAM_START,
            ECHO_RAM_END));
    gameboy -> echo_ram = *echo_ram;
    /*### END CORR ###*/

    // BOOT ROM
    M_REQUIRE_NO_ERR(bootrom_init(&(gameboy -> bootrom)));
    bootrom_plug(&(gameboy -> bootrom), gameboy -> bus);

    gameboy -> boot = 1;

    return ERR_NONE;
}

// ======================================================================
void gameboy_free(gameboy_t* gameboy)
{
    if (gameboy != NULL){
        /*### CORR ###*/
        for (int i = 0; i < gameboy -> nb_components; ++i){
            if (&(gameboy -> components[i]) != NULL){
                bus_unplug(gameboy -> bus, &(gameboy -> components[i]));
                component_free(&(gameboy -> components[i]));
            }
        }
        if (&(gameboy -> echo_ram) != NULL){
            bus_unplug(gameboy -> bus, &(gameboy -> echo_ram));
            component_free(&(gameboy -> echo_ram));
        }
        if (&(gameboy -> cpu) != NULL){
            cpu_free(&(gameboy -> cpu));
        }
        // what to do with bus??
        /*### END CORR ###*/
        gameboy = NULL;
    }
}

// ======================================================================
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    M_REQUIRE_NO_ERR(bootrom_bus_listener(gameboy, gameboy -> cpu.PC));
}