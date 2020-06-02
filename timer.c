#include <stdint.h>
#include "timer.h"
#include "error.h"
#include "bit.h"
#include "bus.h"
#include "gameboy.h"

#define TIMA_MAX_CYCLES 0xFF
// ### CORR: added macro for reading/writing register
#define READ_REG(X,Y) \
    M_REQUIRE_NO_ERR(bus_read(*(timer -> cpu -> bus), REG_ ## X, Y));
#define WRITE_REG(X,Y) \
    M_REQUIRE_NO_ERR(bus_write(*(timer -> cpu -> bus), REG_ ## X, Y));

// =============================== AUX ==================================
int timer_state(gbtimer_t* timer, bit_t* res)
{
    M_REQUIRE_NON_NULL(timer);
    uint8_t tac = 0;
    M_REQUIRE_NO_ERR(bus_read(*(timer -> cpu -> bus), REG_TAC, &tac));
    bit_t counter_bit = 0;
    switch(tac & 0x3){
        case 0: // index 9
            counter_bit = bit_get(msb8(timer -> counter), 9 - 8);
            break;
        case 1: // index 3
            counter_bit = bit_get(lsb8(timer -> counter), 3);
            break;
        case 2: // index 5
            counter_bit = bit_get(lsb8(timer -> counter), 5);
            break;
        case 3: // index 7
            counter_bit = bit_get(lsb8(timer -> counter), 7);
            break;
        default:
            return ERR_BAD_PARAMETER;
    }
    *res = bit_get(tac, 2) & counter_bit;
    return ERR_NONE;
}

int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state)
{
    M_REQUIRE_NON_NULL(timer);
    bit_t new_state = 0;
    M_REQUIRE_NO_ERR(timer_state(timer, &new_state));
    if ((old_state == 1) && (new_state == 0)){
        uint8_t secondary_counter = 0;
        READ_REG(TIMA,&secondary_counter); // ### CORR: use of macro
        WRITE_REG(TIMA,secondary_counter + 1);
        if (secondary_counter == TIMA_MAX_CYCLES){ // ### CORR: removed greater than
            uint8_t reinit = 0;
            READ_REG(TMA,&reinit); // ### CORR: use of macro
            WRITE_REG(TIMA,reinit);
            cpu_request_interrupt(timer -> cpu, TIMER);
        }
    }
    return ERR_NONE;
}

// ======================================================================
int timer_init(gbtimer_t* timer, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(cpu);
    timer -> cpu = cpu;
    timer -> counter = 0;
    return ERR_NONE;
}

// ======================================================================
int timer_cycle(gbtimer_t* timer)
{
    M_REQUIRE_NON_NULL(timer); // ### CORR: null check
    bit_t old_state;
    M_REQUIRE_NO_ERR(timer_state(timer, &old_state));
    timer -> counter = timer -> counter + GB_TICS_PER_CYCLE; // ### CORR: use of macro
    WRITE_REG(DIV,msb8(timer -> counter));
    M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
    return ERR_NONE;
}

// ======================================================================
int timer_bus_listener(gbtimer_t* timer, addr_t addr)
{
    M_REQUIRE_NON_NULL(timer);
    if (addr == REG_DIV || addr == REG_TAC){
        bit_t old_state;
        M_REQUIRE_NO_ERR(timer_state(timer, &old_state));
        if (addr == REG_DIV){
            timer -> counter = 0;
            WRITE_REG(DIV,0);
        }
        M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
    }
    return ERR_NONE;
}