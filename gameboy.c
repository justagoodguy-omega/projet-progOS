#include <stdint.h>
#include "gameboy.h"
#include "component.h"
#include "bus.h"
#include "error.h"
#include "bootrom.h"
#include "timer.h"
#include "cartridge.h"

// ### CORR: modularity on component creation
#define COMP_INIT(i, X) \
    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[i]), MEM_SIZE(X)));
#define COMP_PLUG(i, X) \
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> components[i]), X ## _START, X ## _END));

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

    //CYCLES
    gameboy -> cycles = 1;

    //TIMER
    gameboy -> timer.cpu = &(gameboy -> cpu);
    gameboy -> timer.counter = 0;

    //CARTRIDGE
    M_REQUIRE_NO_ERR(cartridge_init(&(gameboy -> cartridge), filename));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, &(gameboy -> cartridge.c), BANK_ROM0_START,
            BANK_ROM1_END));

    // WORK RAM
    COMP_INIT(0, WORK_RAM);
    COMP_PLUG(0, WORK_RAM);

    // ECHO_RAM
    component_t* echo_ram = (component_t*)malloc(sizeof(component_t));
    M_REQUIRE_NO_ERR(component_shared(echo_ram, &(gameboy -> components[0])));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, echo_ram, ECHO_RAM_START,
            ECHO_RAM_END));
    mem_free(echo_ram -> mem); // ### CORR: free memory of echo ram
    gameboy -> echo_ram = *echo_ram;

     //REGISTERS
    COMP_INIT(1, REGISTERS);
    COMP_PLUG(1, REGISTERS);
    
    //EXTERNAL_RAM
    COMP_INIT(2, EXTERN_RAM);
    COMP_PLUG(2, EXTERN_RAM);

    //VIDEO_RAM
    COMP_INIT(3, VIDEO_RAM);
    COMP_PLUG(3, VIDEO_RAM);

    //GRAPH_RAM
    COMP_INIT(4, GRAPH_RAM);
    COMP_PLUG(4, GRAPH_RAM);
    
    //USELESS
    COMP_INIT(5, USELESS);
    COMP_PLUG(5, USELESS);
    gameboy -> nb_components = GB_NB_COMPONENTS;

    // BOOT ROM
    M_REQUIRE_NO_ERR(bootrom_init(&(gameboy -> bootrom)));
    // ### CORR: error propagation
    M_REQUIRE_NO_ERR(bootrom_plug(&(gameboy -> bootrom), gameboy -> bus));
    gameboy -> boot = 1;

    // SCREEN
    // ### for test week 9 M_REQUIRE_NO_ERR(lcdc_init(gameboy));
    // ### for test week 9 M_REQUIRE_NO_ERR(lcdc_plug(&(gameboy -> screen), gameboy -> bus));

    // JOYPAD
    // ### for test week 9 M_REQUIRE_NO_ERR(joypad_init_and_plug(&(gameboy -> pad), &(gameboy -> cpu)));

    return ERR_NONE;
}

// ======================================================================
void gameboy_free(gameboy_t* gameboy)
{
    if (gameboy != NULL){
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

        //free bootrom
        if(&(gameboy -> bootrom) != NULL){
            bus_unplug(gameboy -> bus, &(gameboy -> bootrom));
            component_free(&(gameboy -> bootrom));
        }
        //free cartridge
        if (&(gameboy -> cartridge) != NULL){
            bus_unplug(gameboy -> bus, &(gameboy -> cartridge.c));
            cartridge_free(&gameboy -> cartridge);
        }
        //free screen
        // ### for test week 9 lcdc_free(&(gameboy -> screen));
        // lcdc unplug ??

        //free pad??

        gameboy = NULL;
    }
}

// ======================================================================
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    M_REQUIRE_NON_NULL(gameboy); // ### CORR: null check
    M_REQUIRE_NO_ERR(timer_cycle(&(gameboy -> timer)));
    M_REQUIRE_NO_ERR(cpu_cycle(&(gameboy -> cpu)));

    M_REQUIRE_NON_NULL(gameboy);
    if(cycle > gameboy -> cycles){
        for(; gameboy -> cycles < cycle; ++(gameboy -> cycles)){
            // ### CORR: listeners in loop
            M_REQUIRE_NO_ERR(bootrom_bus_listener(gameboy, gameboy -> cpu.PC));
            M_REQUIRE_NO_ERR(timer_bus_listener(&(gameboy -> timer), gameboy -> cpu.PC));
            M_REQUIRE_NO_ERR(cpu_cycle(&(gameboy -> cpu)));
        }
    } else {
        return ERR_BAD_PARAMETER; // ### CORR: added error if cycle <= cycles
    }
    return ERR_NONE;
}