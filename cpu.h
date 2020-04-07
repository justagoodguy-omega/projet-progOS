#pragma once

/**
 * @file cpu.h
 * @brief CPU model for PPS-GBemul project, high level interface
 *
 * @author J.-C. Chappelier & C. Hölzl, EPFL
 * @date 2019
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "alu.h"
#include "bus.h"
#include "bit.h"

//=========================================================================
/**
 * @brief Type to represent CPU interupts
 */
typedef enum {
    VBLANK, LCD_STAT, TIMER, SERIAL, JOYPAD
} interrupt_t ;

#define REG_IF          0xFF0F
#define REG_IE          0xFFFF
#define HIGH_RAM_START   0xFF80
#define HIGH_RAM_END     0xFFFE
#define HIGH_RAM_SIZE ((HIGH_RAM_END - HIGH_RAM_START)+1)

//=========================================================================
/**
 * @brief Type to represent registers
 */
 
 typedef uint8_t register_8;
 typedef uint16_t register_16;
 
 

//=========================================================================
/**
 * @brief Type to represent CPU
 */

typedef struct{
    alu_output_t alu;
    bus_t* bus;
    uint8_t idle_time;

    union {
     struct{
         register_8 A;
         register_8 F;
     };
     register_16 AF;
    };
    union {
     struct{
         register_8 B;
         register_8 C;
     };
     register_16 BC;
    };
    union {
     struct{
         register_8 D;
         register_8 E;
     };
     register_16 DE;
    };
    union {
     struct{
         register_8 H;
         register_8 L;
     };
     register_16 HL;
    };
    
    register_16 PC;
    register_16 SP;
} cpu_t;

//=========================================================================
/**
 * @brief Run one CPU cycle
 * @param cpu (modified), the CPU which shall run
 * @param cycle, the cycle number to run, starting from 0
 * @return error code
 */
int cpu_cycle(cpu_t* cpu);


/**
 * @brief Plugs a bus into the cpu
 *
 * @param cpu cpu to plug into
 * @param bus bus to plug
 *
 * @return error code
 */
int cpu_plug(cpu_t* cpu, bus_t* bus);


/**
 * @brief Starts the cpu by initializing all registers at zero
 *
 * @param cpu cpu to start
 *
 * @return error code
 */
int cpu_init(cpu_t* cpu);


/**
 * @brief Frees a cpu
 *
 * @param cpu cpu to free
 */
void cpu_free(cpu_t* cpu);


/**
 * @brief Set an interruption
 */
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i);

/**
 * @brief reads the value of the bus at address addr
 * @param cpu the cpu
 * @param addr the address on the bus
 */
data_t cpu_read_at_idx(const cpu_t* cpu, addr_t addr);

/**
 * @brief reads the 16 bits starting from address addr on the bus
 */
addr_t cpu_read16_at_idx(const cpu_t* cpu, addr_t addr);

/**
 * @brief writes data at the address addr on the bus
 */
int cpu_write_at_idx(cpu_t* cpu, addr_t addr, data_t data);

/**
 * @brief writes the 16 bit data data16 on the bus starting at address addr
 */
int cpu_write16_at_idx(cpu_t* cpu, addr_t addr, addr_t data16);
 


#ifdef __cplusplus
}
#endif
