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
#include "sysc/kernel/sc_wait.h"
#include "sysc/kernel/sc_wait_cthread.h"
using namespace sc_core;

SC_MODULE(MEMORY_CONTROLLER){
    sc_in<bool> clk, r, w, wide, mem_ready;
    sc_in<uint32_t> addr, wdata, mem_rdata;
    sc_in<uint8_t> user;

    sc_signal<bool> rom_read, rom_ready, allowed, mpu_write, mpu_wide;
    sc_signal<uint8_t> mpu_owner;
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

        memory_protection_unit.input_address(addr);
        memory_protection_unit.input_user(user);
        memory_protection_unit.input_write(mpu_write);
        memory_protection_unit.input_wide(mpu_wide);
        memory_protection_unit.output_allowed(allowed);
        memory_protection_unit.output_owner(mpu_owner);

        SC_THREAD(behaviour);
        sensitive << clk.pos();

    }

    void behaviour(){
        while(true){   
            wait();
            ready.write(0);
            error.write(0);
            if(r.read()){
                if(addr.read() < rom_size){
                    rom_read.write(1); //ROM access
                    while(!rom.ready.read()){
                        wait();
                    }
                    rdata.write(rom_rdata.read());
                    rom_read.write(0);
                    while (rom.ready.read()) {
                        wait();
                    }
                    ready.write(1);
                }else{
                    bool allowed = memory_protection_unit.accessAndModify(addr.read(), user.read(), false, wide.read());
                    if(!allowed){
                        ready.write(1);
                        error.write(1);
                        continue;
                    }
                    
                    uint32_t word = doMemRead();
                    if(wide.read()){
                        rdata.write(word);
                    }else{
                        uint32_t byte = word & 0xFF;
                        rdata.write(byte);
                    }
                    ready.write(1);
                }
            }
            if(w.read()){
                if(addr.read() < rom_size){
                    ready.write(1);
                    error.write(1);
                    continue;    
                }else{
                    bool allowed = memory_protection_unit.accessAndModify(addr.read(), user.read(), true, wide.read());
                    if(!allowed){
                        ready.write(1);
                        error.write(1);
                        continue;
                    }
                    
                    uint32_t resWData;

                    if(wide.read()){
                        resWData = wdata.read();
                    }else{
                        uint32_t oldWord = doMemRead();
                        uint8_t newByte = wdata.read() & 0xFF;
                        resWData = (oldWord & 0xFFFFFF00) | newByte;
                    }

                    mem_r.write(0);
                    mem_w.write(1);
                    
                    mem_addr.write(addr.read());
                    mem_wdata.write(resWData);

                    while(!mem_ready.read()){
                        wait();
                    }
                    mem_w.write(0);
                    while(mem_ready.read()){
                        wait();
                    }
                    ready.write(1);
                }
            }
        }
    }

    uint32_t doMemRead(){
        mem_r.write(1);
        mem_w.write(0);
        mem_addr.write(addr.read());
        mem_wdata.write(0);
        while(!mem_ready.read()){
            wait();
        }
        uint32_t res = mem_rdata.read();
        mem_r.write(0);
        while(mem_ready.read()){
            wait();
        }

        return res;
    }

    uint8_t getOwner(uint32_t address){
        return memory_protection_unit.getOwner(address);
    }

    void setRomAt(uint32_t address, uint8_t data){
        rom.setRomAt(address, data);
    }

};


#endif