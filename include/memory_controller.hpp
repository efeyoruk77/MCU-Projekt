#ifndef MEMORY_CONTROLLER_HPP
#define MEMORY_CONTROLLER_HPP

#include <systemc>
#include <systemc.h>
using namespace sc_core;

SC_MODULE(MEMORY_CONTROLLER){
    sc_in<bool> clk, r, w, wide, mem_ready;
    sc_in<uint32_t> addr, wdata, mem_rdata;
    sc_in<uint8_t> user;

    sc_out<uint32_t> rdata, mem_addr, mem_wdata;
    sc_out<bool> ready, error, mem_r, mem_w;

    SC_CTOR(MEMORY_CONTROLLER){
        
    }

};

#endif