#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

#include <systemc>
#include <map>
using namespace sc_core;

#define LATENCY 100

SC_MODULE(MAIN_MEMORY) {
    sc_in<bool> clk, r, w;
    sc_in<uint32_t> addr, wdata;
    
    sc_out<bool> ready;
    sc_out<uint32_t> rdata;

    std::map<uint32_t, uint8_t> my_map;

    SC_CTOR(MAIN_MEMORY){
        SC_THREAD(update);
        sensitive << clk.pos();
    }

    void update(){
        ready.write(false);
        while(true){
            wait();
            if(r.read()){
                uint32_t res = get(addr.read());
                for(int i = 0; i < LATENCY; i++){
                    wait();
                }
                rdata.write(res);
                ready.write(true);
                while(r.read()) wait();
                ready.write(false);
            } else if(w.read()){
                set(addr.read(), wdata.read());
                for(int i = 0; i < LATENCY; i++){
                    wait();
                }
                ready.write(true);
                while(w.read()) wait();
                ready.write(false);
            }
        }
    }

    void doRead(bool write){
        ready.write(false);
        uint32_t res = get(addr.read());
        for(int i = 0; i < LATENCY; i++){
            wait();
        }
        rdata.write(res);
        if(!write) ready.write(true);
    }

    void doWrite(){
        ready.write(false);
        set(addr.read(), wdata.read());
        for(int i = 0; i < LATENCY; i++){
            wait();
        }
        ready.write(true);
    }

    uint32_t get(uint32_t address){
        uint32_t res = 0;
        for(int i = 0; i < 4; i++){
            uint8_t val = 0;
            if(my_map.find(address + i) != my_map.end()){
                val = my_map[address + i];
            }
            res += val << (i * 8);
        }
        return res;
    }

    void set(uint32_t address, uint32_t val){
        for(int i = 0; i < 4; i++){
            my_map[address + i] = (val >> (8 * i)) & 0xFF;
            if(address + i == UINT32_MAX){
                break;
            }
        }
    }
};

#endif // MAIN_MEMORY_HPP