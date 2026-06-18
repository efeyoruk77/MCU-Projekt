#ifndef READ_ONLY_MEMORY_HPP
#define READ_ONLY_MEMORY_HPP

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

    sc_out<uint32_t> rdata;
    sc_out<bool> ready;

    SC_HAS_PROCESS(ReadOnlyMemory);

    ReadOnlyMemory(sc_module_name name, uint32_t latency, uint32_t size, const uint32_t* data) : sc_module(name), latency(latency), size(size), memory(size/4)
    {
        for(int i = 0; i < (size/4); i++){
            memory[i] = data[i];
        }

        SC_THREAD(readMethod);
        sensitive << clk.pos();
    }

    void readMethod();

};
#endif