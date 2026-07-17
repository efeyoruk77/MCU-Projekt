#ifndef READ_ONLY_MEMORY_HPP
#define READ_ONLY_MEMORY_HPP

#include <iostream>
#include <sys/types.h>
#include <systemc.h>
#include <cstdint>

SC_MODULE(ReadOnlyMemory){
    
private:
    uint32_t latency;
    uint32_t size;
    std::vector<uint32_t> memory;
    uint32_t counter;

public:
    sc_in<uint32_t> addr;
    sc_in<bool> clk;
    sc_in<bool> read_enable;
    sc_in<bool> wide;

    sc_out<uint32_t> rdata;
    sc_out<bool> ready;

    SC_HAS_PROCESS(ReadOnlyMemory);

    ReadOnlyMemory(sc_module_name name, uint32_t latency, uint32_t size, const uint32_t* data) : sc_module(name), latency(latency), size(size), memory(size/4), counter(0)
    {   
        if(data != NULL){
            for(uint32_t i = 0; i < (size / 4); i++){
                    memory[i] = data[i];
                }
        }
        SC_THREAD(readMethod);
        sensitive << clk.pos();
    }

    void readMethod();

    void setRomAt(uint32_t address, uint8_t data){
        uint32_t index = address/4;
        uint32_t offset = address % 4;

        if(index >= memory.size()){
            std::cerr << "Invalid address to set ROM" << std::endl;
            return; //No need to exit the program, we exit the method instead
        }
        uint32_t shift = offset * 8;
        uint32_t bitMask = ~((static_cast<uint32_t>(0xFF)) << shift);

        memory[index] = (memory[index] & bitMask) | (static_cast<uint32_t>(data) << shift);
    }

    uint32_t getByte(uint32_t address){
        uint32_t wordIndex = address / 4;
        uint32_t byteOffset = address % 4;
        if(wordIndex >= memory.size()) return 0;
        return (memory[wordIndex] >> (8 * byteOffset)) & 0xFF; 
    }
};
#endif