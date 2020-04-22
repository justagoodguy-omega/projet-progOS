#include <stdint.h>
#include "gameboy.h"
#include "error.h"
#include "gameboy.h"

/**
 * @brief Creates a gameboy
 *
 * @param gameboy pointer to gameboy to create
 */
int gameboy_create(gameboy_t* gameboy, const char* filename)
{
    M_REQUIRE_NON_NULL(gameboy);

    // WORK_RAM
    //char* wr = "WORK_RAM";
    // ### WORK_RAM_END not recognized when using MEM_SIZE
    memory_t* work_mem = calloc(WORK_RAM_END - WORK_RAM_START + 1, sizeof(uint8_t));
    M_REQUIRE_NON_NULL(work_mem);

    component_t* workRAM = malloc(sizeof(component_t));
    M_REQUIRE_NON_NULL(workRAM);

    workRAM -> mem = &work_mem;
    workRAM -> start = WORK_RAM_START;
    workRAM -> end = WORK_RAM_END;
    
    // ECHO_RAM
    component_t* echoRAM = malloc(sizeof(component_t));
    M_REQUIRE_NON_NULL(echoRAM);
    M_REQUIRE_NO_ERR(component_shared(echoRAM, workRAM));
    echoRAM -> start = ECHO_RAM_START;
    echoRAM -> end = ECHO_RAM_END;

    // BUS
    bus_t gb_bus;
    M_REQUIRE_NO_ERR(bus_plug(gb_bus, workRAM, workRAM -> start, workRAM -> end));
    M_REQUIRE_NO_ERR(bus_plug(gb_bus, echoRAM, echoRAM -> start, echoRAM -> end));
    gameboy -> bus = gb_bus;
    gameboy -> components[0] = workRAM;
    gameboy -> components[1] = echoRAM;

    return ERR_NONE;
}

/**
 * @brief Destroys a gameboy
 *
 * @param gameboy pointer to gameboy to destroy
 */
void gameboy_free(gameboy_t* gameboy)
{
    M_REQUIRE_NON_NULL(gameboy);

    for (int i = 0; i < GB_NB_COMPONENTS; ++i){
        if (components[i] != NULL){
            component_free(components[i]);
            components[i] = NULL;
        }
    }

    gameboy = NULL;
}