#ifndef MEMORY_PROTECTION_UNIT_HPP
#define MEMORY_PROTECTION_UNIT_HPP

#include <cstdint>
#include <unordered_map>
#include <systemc.h>

SC_MODULE(MemoryProtectionUnit){
private:
    uint32_t block_size;
    std::unordered_map<uint32_t, sc_uint<8>> owner_map;

public:
    sc_in<uint32_t> input_address;
    sc_in<sc_uint<8>> input_user;
    sc_in<bool> input_write;
    sc_in<bool> input_wide;

    sc_out<bool> output_allowed;
    sc_out<sc_uint<8>> output_owner;

    
    SC_HAS_PROCESS(MemoryProtectionUnit);

    MemoryProtectionUnit(sc_module_name name, uint32_t bs) : sc_module(name), block_size(bs)
    {
        SC_METHOD(checkAccess);
        sensitive << input_address << input_user << input_write << input_wide;
    }

    void checkAccess();
    bool accessAndModify(uint32_t address, sc_uint<8> user, bool write, bool wide);
    sc_uint<8> getOwner(uint32_t address) const;
};

#endif