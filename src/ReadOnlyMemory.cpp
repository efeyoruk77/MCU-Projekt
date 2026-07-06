#include "../include/ReadOnlyMemory.hpp"
#include <cstdint>

void ReadOnlyMemory::readMethod(){
    ready.write(false);
    while(true){
        if(read_enable.read() == true){
            uint32_t address = addr.read();
            for(uint32_t i = 0; i < latency; i++){
                wait();
            }
            uint32_t completeAddress = address / 4;
            if(completeAddress < memory.size()){
                rdata.write(memory[completeAddress]);
            } else{
                rdata.write(0);
            }
            ready.write(true);
            while(read_enable.read() == true) wait();
            ready.write(false);
        }
        else{
            ready.write(false);
            wait();
        }
    }
}