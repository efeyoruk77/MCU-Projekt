#include "../include/ReadOnlyMemory.hpp"
#include "sysc/kernel/sc_wait.h"
#include <cstdint>
#include <cstdio>

void ReadOnlyMemory::readMethod(){
    ready.write(false);
    while(true){
        if(read_enable.read() == true){
            uint32_t address = addr.read();
            uint32_t index = address / 4;
            
            if(index >= memory.size()){ //this block should never execute considering we implement MCU properly
                rdata.write(0);
                std::cerr << "Invalid ROM access" << std::endl;
                ready.write(true);
                while (read_enable.read()) wait();
                ready.write(false);
                continue;
            }

            for(uint32_t i = 0; i < latency; i++){
                wait();
            }
            if(wide.read()){
                uint32_t b0 = getByte(address);
                uint32_t b1 = getByte(address + 1);
                uint32_t b2 = getByte(address + 2);
                uint32_t b3 = getByte(address + 3);
                uint32_t word = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
                rdata->write(word); 
            } else{
                rdata.write(getByte(address));
            }

            ready.write(true);
            while(read_enable.read() == true) wait();
            ready.write(false);

        }else{
            ready.write(false);
            wait();
        }
    }
}