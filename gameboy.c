#include <stdint.h>
#include "gameboy.h"
#include "component.h"
#include "bus.h"
#include "error.h"
#include "bootrom.h"
#include "timer.h"
#include "cartridge.h"


//number of cycles simulated
uint64_t cycles = 0;

// ======================================================================
int gameboy_create(gameboy_t* gameboy, const char* filename)
{
    M_REQUIRE_NON_NULL(gameboy);

    //BUS
    for (int i = 0; i < BUS_SIZE; ++i){
        gameboy -> bus[i] = 0;
    }

    //CPU
    M_REQUIRE_NO_ERR(cpu_init(&(gameboy -> cpu)));
    M_REQUIRE_NO_ERR(cpu_plug(&(gameboy -> cpu), &(gameboy -> bus)));

    //TIMER
    gameboy -> timer.cpu = &(gameboy -> cpu);
    gameboy -> timer.counter = 0;

    //CARTRIDGE
    M_REQUIRE_NO_ERR(cartridge_init(&(gameboy -> cartridge), filename));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> cartridge.c), BANK_ROM0_START,
            BANK_ROM1_END));

    // WORK RAM
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[0]),
                MEM_SIZE(WORK_RAM)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[0]),
                WORK_RAM_START, WORK_RAM_END));

    // ECHO_RAM
    component_t* echo_ram = (component_t*)malloc(sizeof(component_t));
    M_REQUIRE_NO_ERR(component_shared(echo_ram, &(gameboy -> components[0])));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, echo_ram, ECHO_RAM_START,
            ECHO_RAM_END));
    gameboy -> echo_ram = *echo_ram;

     //REGISTERS
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[1]),
                MEM_SIZE(REGISTERS)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[1]),
                REGISTERS_START, REGISTERS_END));
    
    //EXTERNAL_RAM
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[2]),
                MEM_SIZE(EXTERN_RAM)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[2]),
                EXTERN_RAM_START, EXTERN_RAM_END));

    //VIDEO_RAM
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[3]),
                MEM_SIZE(VIDEO_RAM)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[3]),
                VIDEO_RAM_START, VIDEO_RAM_END));

    //GRAPH_RAM
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[4]),
                MEM_SIZE(GRAPH_RAM)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[4]),
                GRAPH_RAM_START, GRAPH_RAM_END));
    
    //USELESS
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[5]),
                MEM_SIZE(USELESS)));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[5]),
                USELESS_START, USELESS_END));
    gameboy -> nb_components = GB_NB_COMPONENTS;

    // BOOT ROM
    M_REQUIRE_NO_ERR(bootrom_init(&(gameboy -> bootrom)));
    bootrom_plug(&(gameboy -> bootrom), gameboy -> bus);
    gameboy -> boot = 1;

    // SCREEN
    M_REQUIRE_NO_ERR(lcdc_init(&(gameboy -> screen)));
    M_REQUIRE_NO_ERR(lcdc_plug(&(gameboy -> screen), gameboy -> bus));

    // JOYPAD
    M_REQUIRE_NO_ERR(joypad_init_and_plug(&(gameboy -> pad), &(gameboy -> cpu)));

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
        //free echo_ram
        if (&(gameboy -> echo_ram) != NULL){
            bus_unplug(gameboy -> bus, &(gameboy -> echo_ram));
            component_free(&(gameboy -> echo_ram));
        }
        //free cpu
        if (&(gameboy -> cpu) != NULL){
            cpu_free(&(gameboy -> cpu));
        }
        // what to do with bus??
        /*### END CORR ###*/

        //free bootrom
        if(&(gameboy -> bootrom) != NULL){
            bus_unplug(gameboy -> bus, &(gameboy -> bootrom));
            component_free(&(gameboy -> bootrom));
        }
        //free cartridge
        if (&(gameboy -> cartridge) != NULL){
            bus_unplug(gameboy -> bus, &(gameboy -> cartridge));
            cartridge_free(&gameboy -> cartridge);
        }
        //free screen
        lcdc_free(&(gameboy -> screen));
        // lcdc unplug ??

        //free pad??

        gameboy = NULL;
    }
}

// ======================================================================
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    //M_REQUIRE_NO_ERR(bootrom_bus_listener(gameboy, gameboy -> cpu.PC));
    M_REQUIRE_NO_ERR(timer_cycle(&(gameboy -> timer)));
    M_REQUIRE_NO_ERR(cpu_cycle(&(gameboy -> cpu)));
    M_REQUIRE_NO_ERR(timer_bus_listener(&(gameboy -> timer), gameboy -> cpu.PC));

    M_REQUIRE_NON_NULL(gameboy);
    if(cycle > cycles){
        for(; cycles < cycle; ++cycles){
            M_REQUIRE_NO_ERR(cpu_cycle(&(gameboy -> cpu)));
        }
    }
    return ERR_NONE;
}