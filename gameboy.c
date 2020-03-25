#pragma once

/**
 * @file gameboy.c
 * @brief Gameboy Emulator
 * 
 * @author L. Rovati, P. Oliver, EPFL
 * @date 2020
 */ 

#include <stdint.h>
#include "gameboy.h"
#include "error.h"

/**
 * @brief Creates a gameboy
 *
 * @param gameboy pointer to gameboy to create
 */
int gameboy_create(gameboy_t* gameboy, const char* filename){

    gameboy = malloc(sizeof(gameboy_t));
    M_REQUIRE_NON_NULL(gameboy);

    memory_t* mem = calloc(8192, sizeof(uint8_t));
    M_REQUIRE_NON_NULL(mem);

    component_t* workRAM = malloc(sizeof(component_t);
    M_REQUIRE_NON_NULL(workRAM);

    workRAM -> mem = &mem;
    workRAM -> start = WORK_RAM_START;
    workRAM -> end = WORK_RAM_END;
    bus_t gb_bus;
    M_REQUIRE_NO_ERR(bus_plug(gb_bus, &workRAM, workRAM -> start, workRAM -> end));

    gameboy -> bus = gb_bus; 
    gameboy -> components[0] = &workRAM;

    return ERR_NONE;
}

/**
 * @brief Destroys a gameboy
 *
 * @param gameboy pointer to gameboy to destroy
 */
void gameboy_free(gameboy_t* gameboy){

    M_REQUIRE_NON_NULL(gameboy);

    for (int i = 0; i < GB_NB_COMPONENTS; ++i){
        free(components[i] -> mem);
        components[i] -> mem = NULL;
        free(components[i]);
        components[i] = NULL;
    }

    free(gameboy);
    gameboy = NULL;

}