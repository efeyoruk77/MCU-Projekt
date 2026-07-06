#ifndef MEMORY_CONTROLLER_HPP
#define MEMORY_CONTROLLER_HPP

#include <cstdint>
#include <sys/types.h>
#include <systemc>
#include <systemc.h>
#include "MemoryProtectionUnit.hpp"
#include "ReadOnlyMemory.hpp"
#include "sysc/kernel/sc_module_name.h"
#include "sysc/kernel/sc_simcontext.h"
using namespace sc_core;

SC_MODULE(MEMORY_CONTROLLER){
    sc_in<bool> clk, r, w, wide, mem_ready;
    sc_in<uint32_t> addr, wdata, mem_rdata;
    sc_in<uint8_t> user;

    sc_out<uint32_t> rdata, mem_addr, mem_wdata;
    sc_out<bool> ready, error, mem_r, mem_w;

    ReadOnlyMemory rom;
    MemoryProtectionUnit memory_protection_unit;
    SC_HAS_PROCESS(MEMORY_CONTROLLER);

    MEMORY_CONTROLLER(sc_module_name name, uint32_t rom_latency, uint32_t rom_size, uint32_t block_size, const uint32_t* rom_content) :
    sc_module(name), rom("ROM", rom_latency, rom_size, rom_content), memory_protection_unit("memory_protection_unit", block_size){
        
        

    }

    uint8_t getOwner(uint32_t address){
        return memory_protection_unit.getOwner(address).to_uint();
    }

    void setRomAt(uint32_t address, uint8_t data){

    }

};


#endif