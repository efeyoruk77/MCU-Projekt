#ifndef MEMORY_CONTROLLER_HPP
#define MEMORY_CONTROLLER_HPP

#include <cstdint>
#include <sys/types.h>
#include <systemc>
#include <systemc.h>
#include "MemoryProtectionUnit.hpp"
#include "ReadOnlyMemory.hpp"
#include "sysc/communication/sc_signal.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_module_name.h"
#include "sysc/kernel/sc_simcontext.h"
using namespace sc_core;

SC_MODULE(MEMORY_CONTROLLER){
    sc_in<bool> clk, r, w, wide, mem_ready;
    sc_in<uint32_t> addr, wdata, mem_rdata;
    sc_in<uint8_t> user;

    sc_signal<bool> rom_read, rom_ready;
    sc_signal<uint32_t> rom_rdata;

    sc_out<uint32_t> rdata, mem_addr, mem_wdata;
    sc_out<bool> ready, error, mem_r, mem_w;
   
    uint32_t rom_size;

    ReadOnlyMemory rom;
    MemoryProtectionUnit memory_protection_unit;
    SC_HAS_PROCESS(MEMORY_CONTROLLER);

    MEMORY_CONTROLLER(sc_module_name name, uint32_t latency_rom, uint32_t rom_size, uint32_t block_size, const uint32_t* rom_content) :
    sc_module(name), rom("ROM", latency_rom, rom_size, rom_content), memory_protection_unit("memory_protection_unit", block_size){
        this->rom_size = rom_size;
        rom.read_enable(rom_read);
        rom.addr(addr);
        rom.wide(wide);
        rom.clk(clk);

        rom.rdata(rom_rdata);
        rom.ready(rom_ready);
        SC_THREAD(behaviour);
    

    }

    void behaviour(){
        while(true){   
            wait();
            if(r.read()){
                if(addr.read() < rom_size){
                    rom_read.write(1); //ROM access
                    while(!rom.ready.read()){
                        wait();
                    }
                    
                }
            }
        }
    }

    uint8_t getOwner(uint32_t address){
        return memory_protection_unit.getOwner(address).to_uint();
    }

    void setRomAt(uint32_t address, uint8_t data){
        rom.setRomAt(address, data);
    }

};


#endif