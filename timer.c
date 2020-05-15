#include <stdint.h>
#include "timer.h"
#include "error.h"
#include "bit.h"
#include "bus.h"

#define TIMA_MAX_CYCLES 0xFF

// =============================== AUX ==================================
int timer_state(gbtimer_t* timer, bit_t* res)
{
    M_REQUIRE_NON_NULL(timer);
    uint8_t tac = 0;
    M_REQUIRE_NO_ERR(bus_read(*(timer -> cpu -> bus), REG_TAC, &tac));
    uint8_t index = 0;
    switch(tac & 0x3){
        case 0:
            index = 9;
            break;
        case 1:
            index = 3;
            break;
        case 2:
            index = 5;
            break;
        case 3:
            index = 7;
            break;
        default:
            return ERR_BAD_PARAMETER;
    }
    uint8_t main_counter = 0;
    M_REQUIRE_NO_ERR(bus_read(*(timer -> cpu -> bus), REG_DIV, &main_counter));
    *res = bit_get(tac, 2) & bit_get(main_counter, index);
    return ERR_NONE;
}

int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state)
{
    M_REQUIRE_NON_NULL(timer);
    bit_t new_state = 0;
    M_REQUIRE_NO_ERR(timer_state(timer, &new_state));
    if ((old_state == 1) && (new_state == 0)){
        uint8_t secondary_counter = 0;
        M_REQUIRE_NO_ERR(bus_read(*(timer -> cpu -> bus), REG_TIMA,
                &secondary_counter));
        M_REQUIRE_NO_ERR(bus_write(*(timer -> cpu -> bus), REG_TIMA,
                secondary_counter + 1));
        if (secondary_counter >= TIMA_MAX_CYCLES){
            uint8_t reinit = 0;
            M_REQUIRE_NO_ERR(bus_read(*(timer -> cpu -> bus), REG_TMA, &reinit));
            M_REQUIRE_NO_ERR(bus_write(*(timer -> cpu -> bus), REG_TIMA, reinit));
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
    bit_t old_state;
    M_REQUIRE_NO_ERR(timer_state(timer, &old_state));
    timer -> counter = timer -> counter + 4;
    M_REQUIRE_NO_ERR(bus_write(*(timer -> cpu -> bus), REG_DIV,
            msb8(timer -> counter)));
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
            M_REQUIRE_NO_ERR(bus_write(*(timer -> cpu -> bus), REG_DIV, 0));
        }
        M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
    }
    return ERR_NONE;
}