#include <stdint.h>
#include "timer.h"
#include "error.h"
#include "bit.h"

// =============================== AUX ==================================
int timer_state(gbtimer_t* timer, bit_t* res)
{
    M_REQUIRE_NON_NULL(timer);
    uint8_t tac;
    M_REQUIRE_NO_ERR(bus_read(timer -> cpu -> bus, REG_TAC, &tac));
    uint8_t main_counter;
    M_REQUIRE_NO_ERR(bus_read(timer -> cpu -> bus, REG_DIV, &main_counter));
    *res = bit_get(tac, 2) & bit_get(main_counter, tac & 0x3);
    return ERR_NONE;
}

int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state)
{
    M_REQUIRE_NON_NULL(timer);
    bit_t new_state;
    M_REQUIRE_NO_ERR(timer_state(timer, &new_state));
    if (old_state == 1 &  new_state == 0){
        uint8_t secondary_counter;
        M_REQUIRE_NO_ERR(bus_read(timer -> cpu -> bus, REG_TIMA,
                &secondary_counter));
        if (secondary_counter == 0xFF){
            uint8_t reinit;
            M_REQUIRE_NO_ERR(bus_read(timer -> cpu -> bus, REG_TMA, &reinit));
            M_REQUIRE_NO_ERR(bus_write(timer -> cpu -> bus, REG_TIMA, reinit));
            cpu_request_interrupt(timer -> cpu, TIMER);
        } else {
            M_REQUIRE_NO_ERR(bus_write(timer -> cpu -> bus, REG_TMA,
                    secondary_counter + 1));
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
    bit_t old_state;
    M_REQUIRE_NO_ERR(timer_state(timer, old_state));
    timer -> counter = timer -> counter + 4;
    M_REQUIRE_NO_ERR(bus_write(timer -> cpu -> bus, REG_DIV,
            msb8(timer -> counter)));
    M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
    return ERR_NONE;
}

// ======================================================================
int timer_bus_listener(gbtimer_t* timer, addr_t addr)
{
    M_REQUIRE_NON_NULL(timer);
    switch(addr){
        case REG_DIV:{
            bit_t old_state;
            M_REQUIRE_NO_ERR(timer_state(timer, old_state));
            timer -> counter = 0;
            M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
            break;
        }
        case REG_TAC:{
            bit_t old_state;
            M_REQUIRE_NO_ERR(timer_state(timer, old_state));
            M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
            break;
        }
        default:
            break;
    }
    return ERR_NONE;
}