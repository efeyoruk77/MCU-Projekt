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
            uint32_t offset = address % 4;
            
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
            uint32_t word = memory[index];
            if(wide.read()){
                rdata.write(word);
            } else{
                uint32_t byte = (word >> (8 * offset)) & 0xFF;
                rdata.write(byte);
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