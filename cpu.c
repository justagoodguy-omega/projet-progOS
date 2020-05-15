/**
 * @file cpu.c
 * @brief Game Boy CPU simulation
 *
 * @date 2019
 */

#include "error.h"
#include "opcode.h"
#include "cpu.h"
#include "cpu-alu.h"
#include "cpu-registers.h"
#include "cpu-storage.h"
#include "util.h"
#include "bus.h"

#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf

// ======================================================================
/**
 * @brief Starts the cpu by initializing all registers at zero
 *
 * @param cpu cpu to start
 *
 * @return error code
 */
int cpu_init(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    cpu_BC_set(cpu, 0);
    cpu_DE_set(cpu, 0);
    cpu_HL_set(cpu, 0);
    cpu_AF_set(cpu, 0);
    cpu -> PC = 0;
    cpu -> SP = 0;

    cpu -> alu.flags = 0;
    cpu -> alu.value = 0;
    cpu -> bus = NULL;
    cpu -> idle_time = 0;
    cpu -> write_listener = 0;
   
    return ERR_NONE;
}

// ======================================================================
/**
 * @brief Plugs a bus into the cpu
 *
 * @param cpu cpu to plug into
 * @param bus bus to plug
 *
 * @return error code
 */
int cpu_plug(cpu_t* cpu, bus_t* bus)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(bus);
    cpu -> bus = bus;

    bus_plug(*bus, &(cpu -> high_ram), HIGH_RAM_START, HIGH_RAM_END);

    (*bus)[REG_IF] = &cpu -> IF;
    (*bus)[REG_IE] = &cpu -> IE;
    return ERR_NONE;
}

// ======================================================================
/**
 * @brief Frees a cpu
 *
 * @param cpu cpu to free
 */
void cpu_free(cpu_t* cpu)
{
    if (cpu != NULL){
        bus_unplug(*(cpu -> bus), &(cpu -> high_ram));
        component_free(&(cpu -> high_ram));

        cpu -> bus = NULL; 
    }
}


//=========================================================================


//returns 0 if cc conditions are false, 1 if true
int checkCCconditions(cpu_t* cpu, opcode_t opcode){
    uint8_t cc = extract_cc(opcode);
    if(cc == cc_NZ){
        return !get_Z(cpu -> F);
    }
    else if(cc == cc_Z){
        return get_Z(cpu -> F);
    }
    else if(cc == cc_NC){
        return !get_C(cpu -> F);
    }
    else if(cc == cc_C){
        return get_C(cpu -> F);
    }
    else{
        fprintf(stderr, "impossible cc value in opcode??");
        return ERR_BAD_PARAMETER;
    }
}



//======================================================================

/**
 * @brief Get the index of the interrupt to be handled
 * @param cpu the cpu handling the exception
 * @return the index of the first interrupt to handle
*/
int find_interrupt_index(cpu_t* cpu){
    int interruptionNb = 5;
    for (int i = 0; i < interruptionNb; ++i) {
        int mask = 1 << i;
        if((mask & cpu -> IE) && (mask & cpu -> IF)){
            return i;
        }
    }
    fprintf(stderr, "Interruption index problem");
    return -1;
}

//=========================================================================
/**
 * @brief Executes an instruction
 * @param lu instruction
 * @param cpu, the CPU which shall execute
 * @return error code
 *
 * See opcode.h and cpu.h
 */
static int cpu_dispatch(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(lu);
    M_REQUIRE_NON_NULL(cpu);

    cpu -> alu.value = 0;
    cpu -> alu.flags = 0;

    switch (lu->family) {

    // ALU
    case ADD_A_HLR:
    case ADD_A_N8:
    case ADD_A_R8:
    case INC_HLR:
    case INC_R8:
    case ADD_HL_R16SP:
    case INC_R16SP:
    case SUB_A_HLR:
    case SUB_A_N8:
    case SUB_A_R8:
    case DEC_HLR:
    case DEC_R8:
    case DEC_R16SP:
    case AND_A_HLR:
    case AND_A_N8:
    case AND_A_R8:
    case OR_A_HLR:
    case OR_A_N8:
    case OR_A_R8:
    case XOR_A_HLR:
    case XOR_A_N8:
    case XOR_A_R8:
    case CPL:
    case CP_A_HLR:
    case CP_A_N8:
    case CP_A_R8:
    case SLA_HLR:
    case SLA_R8:
    case SRA_HLR:
    case SRA_R8:
    case SRL_HLR:
    case SRL_R8:
    case ROTCA:
    case ROTA:
    case ROTC_HLR:
    case ROT_HLR:
    case ROTC_R8:
    case ROT_R8:
    case SWAP_HLR:
    case SWAP_R8:
    case BIT_U3_HLR:
    case BIT_U3_R8:
    case CHG_U3_HLR:
    case CHG_U3_R8:
    case LD_HLSP_S8:
    case DAA:
    case SCCF:
        M_EXIT_IF_ERR(cpu_dispatch_alu(lu, cpu));
        break;

    // STORAGE
    case LD_A_BCR:
    case LD_A_CR:
    case LD_A_DER:
    case LD_A_HLRU:
    case LD_A_N16R:
    case LD_A_N8R:
    case LD_BCR_A:
    case LD_CR_A:
    case LD_DER_A:
    case LD_HLRU_A:
    case LD_HLR_N8:
    case LD_HLR_R8:
    case LD_N16R_A:
    case LD_N16R_SP:
    case LD_N8R_A:
    case LD_R16SP_N16:
    case LD_R8_HLR:
    case LD_R8_N8:
    case LD_R8_R8:
    case LD_SP_HL:
    case POP_R16:
    case PUSH_R16:
        M_EXIT_IF_ERR(cpu_dispatch_storage(lu, cpu));
        break;


     // JUMP
    case JP_CC_N16:
        if(checkCCconditions(cpu, lu -> opcode)){
            cpu -> PC = cpu_read_addr_after_opcode(cpu);
            cpu -> idle_time = lu -> cycles + lu -> xtra_cycles;
            return ERR_NONE;
        }
        break;

    case JP_HL:
        cpu -> PC = cpu -> HL;
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;

    case JP_N16:
        cpu -> PC = cpu_read_addr_after_opcode(cpu);
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;

    case JR_CC_E8:
        if(checkCCconditions(cpu, lu -> opcode)){
            cpu -> PC = (cpu -> PC) + lu -> bytes + (int8_t)cpu_read_data_after_opcode(cpu);
            cpu -> idle_time = lu -> cycles + lu -> xtra_cycles;
            return ERR_NONE;
        }
        break;

    case JR_E8:
        cpu -> PC = (cpu -> PC) + lu -> bytes + (int8_t)cpu_read_data_after_opcode(cpu);
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;


    // CALLS
    case CALL_CC_N16:
        if(checkCCconditions(cpu, lu -> opcode)){
            cpu_SP_push(cpu, cpu -> PC + lu -> bytes);
            cpu -> PC = cpu_read_addr_after_opcode(cpu);
            cpu -> idle_time = lu -> cycles + lu -> xtra_cycles;
            return ERR_NONE;
        }
        break;

    case CALL_N16:
        cpu_SP_push(cpu, cpu -> PC + lu -> bytes);
        cpu -> PC = cpu_read_addr_after_opcode(cpu);
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;


    // RETURN (from call)
    case RET:
        cpu -> PC = cpu_SP_pop(cpu);
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;

    case RET_CC:
        if(checkCCconditions(cpu, lu -> opcode)){
            cpu -> PC = cpu_SP_pop(cpu);
            cpu -> idle_time = lu -> cycles + lu -> xtra_cycles;
            return ERR_NONE;
        }
        break;

    case RST_U3:
        cpu_SP_push(cpu, cpu -> PC + lu -> bytes);
        cpu -> PC = extract_n3(lu -> opcode) << 3;
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;


    // INTERRUPT & MISC.
    case EDI:{
        uint8_t opcodeEI = 0xFB;
        uint8_t opcodeDI = 0xF3;
        if(lu -> opcode == opcodeEI){
            cpu -> IME = 1;
        }
        else if(lu -> opcode == opcodeDI){
            cpu -> IME = 0;
        }
    } break;
    

    case RETI:
        cpu -> IME = 1;
        cpu -> PC = cpu_SP_pop(cpu);
        cpu -> idle_time = lu -> cycles;
        return ERR_NONE;
        break;

    case HALT:
        cpu -> HALT = 1;
        break;

    case STOP:
    case NOP:
        // ne rien faire
        break;

    default: {
        fprintf(stderr, "Unknown instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
    } break;

    } // switch

    cpu -> idle_time = cpu -> idle_time + lu -> cycles;
    cpu -> PC = cpu -> PC + lu -> bytes;

    return ERR_NONE;
}


// ----------------------------------------------------------------------
static int cpu_do_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    if(cpu -> IME && (((cpu -> IF) & (cpu -> IE)) != 0)) {
            cpu -> IME = 0;
            int interruptIdx = find_interrupt_index(cpu);
            uint8_t mask = 1 << interruptIdx;
            mask = ~mask;
            cpu -> IF = cpu -> IF & mask;
            cpu_SP_push(cpu, cpu -> PC);

            addr_t handlerAddr = 0x40 + 8*interruptIdx;
            cpu -> PC = handlerAddr;
            cpu -> idle_time += 5;

            return ERR_NONE;
    }
    else {
        opcode_t next_op = cpu_read_at_idx(cpu, cpu -> PC);
        if (next_op == 0xCB){
            instruction_t next_instruction = instruction_prefixed[cpu_read_data_after_opcode(cpu)];
            M_REQUIRE_NO_ERR(cpu_dispatch(&next_instruction, cpu));
        } else {
            instruction_t next_instruction = instruction_direct[next_op];
            M_REQUIRE_NO_ERR(cpu_dispatch(&next_instruction, cpu));
        }

        return ERR_NONE;
    }
}

// ======================================================================
/**
 * See cpu.h
 */
int cpu_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    cpu -> write_listener = 0;
    if(cpu -> idle_time == 0){
        if(cpu -> HALT){
            if (((cpu -> IF) & (cpu -> IE)) != 0){
                cpu -> HALT = 0;
                cpu_do_cycle(cpu);
            }
        }
        else {
            cpu_do_cycle(cpu);
        }
    }
    else {
        cpu -> idle_time = (cpu -> idle_time) - 1;
    }

    return ERR_NONE;
}

// =====================================================================

void cpu_request_interrupt(cpu_t* cpu, interrupt_t i) {
    uint8_t mask = 1 << i;
    cpu -> IF = cpu -> IF | mask;
}
