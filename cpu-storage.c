/**
 * @file cpu-registers.c
 * @brief Game Boy CPU simulation, register part
 *
 * @date 2019
 */

#include "error.h"
#include "cpu-storage.h" // cpu_read_at_HL
#include "cpu-registers.h" // cpu_BC_get
#include "gameboy.h" // REGISTER_START
#include "util.h"
#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf

// ==== see cpu-storage.h ========================================
data_t cpu_read_at_idx(const cpu_t* cpu, addr_t addr)
{
<<<<<<< HEAD
    data_t readData = 0;
    M_REQUIRE_NO_ERR(bus_read(cpu -> bus, addr, &readData));
    return readData; 
=======
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu -> bus);
    data_t readData = 0;
    if(bus_read(*(cpu -> bus), addr, &readData) == 0){
        return readData;
    } else {
        return 0;
    }
    
>>>>>>> master
}

// ==== see cpu-storage.h ========================================
addr_t cpu_read16_at_idx(const cpu_t* cpu, addr_t addr)
{
<<<<<<< HEAD
=======
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu -> bus);
>>>>>>> master
    if (addr == 0xFFFF){
        return 0xFF;
    }
    addr_t readData16 = 0;
<<<<<<< HEAD
    M_REQUIRE_NO_ERR(bus_read16, (cpu -> bus, addr, &readData16));
    return readData16;
=======
    if(bus_read16(*(cpu -> bus), addr, &readData16) == 0){
        return readData16;
    } else {
        return 0;
    }
>>>>>>> master
}

// ==== see cpu-storage.h ========================================
int cpu_write_at_idx(cpu_t* cpu, addr_t addr, data_t data)
{
<<<<<<< HEAD
    M_REQUIRE_NO_ERR(bus_write(cpu -> bus, addr, data));
    return ERR_NONE;
=======
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu -> bus);
    return bus_write(*(cpu -> bus), addr, data);
>>>>>>> master
}

// ==== see cpu-storage.h ========================================
int cpu_write16_at_idx(cpu_t* cpu, addr_t addr, addr_t data16)
{
<<<<<<< HEAD
    M_REQUIRE_NO_ERR(bus_write16(cpu -> bus, addr, data16));
    return ERR_NONE;
=======
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu -> bus);
    return bus_write16(*(cpu -> bus), addr, data16);
>>>>>>> master
}

// ==== see cpu-storage.h ========================================
int cpu_SP_push(cpu_t* cpu, addr_t data16)
{
    addr_t newAddr = cpu -> SP - 2;
    cpu -> SP = newAddr;
    M_REQUIRE_NO_ERR(cpu_write16_at_idx(cpu, newAddr, data16));
    return ERR_NONE;
}

// ==== see cpu-storage.h ========================================
addr_t cpu_SP_pop(cpu_t* cpu)
{
    addr_t readData = cpu_read16_at_idx(cpu, cpu -> SP);
    cpu -> SP = cpu -> SP + 2;
    return readData;
}

// ==== see cpu-storage.h ========================================
int cpu_dispatch_storage(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    switch (lu->family) {
    case LD_A_BCR:
        break;

    case LD_A_CR:
        break;

    case LD_A_DER:
        break;

    case LD_A_HLRU:
        break;

    case LD_A_N16R:
        break;

    case LD_A_N8R:
        break;

    case LD_BCR_A:
        break;

    case LD_CR_A:
        break;

    case LD_DER_A:
        break;

    case LD_HLRU_A:
        break;

    case LD_HLR_N8:
        break;

    case LD_HLR_R8:
        break;

    case LD_N16R_A:
        break;

    case LD_N16R_SP:
        break;

    case LD_N8R_A:
        break;

    case LD_R16SP_N16:
        break;

    case LD_R8_HLR:
        break;

    case LD_R8_N8:
        break;

    case LD_R8_R8: {
    } break;

    case LD_SP_HL:
        break;

    case POP_R16:
        break;

    case PUSH_R16:
        break;

    default:
        fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
        break;
    } // switch

    return ERR_NONE;
}