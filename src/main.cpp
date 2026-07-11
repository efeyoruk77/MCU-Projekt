#include <systemc>
#include <systemc.h>
#include "../include/types.hpp"
#include "../include/memory_controller.hpp"
#include "../include/main_memory.hpp"
using namespace sc_core;

struct Result runSimulation (
    uint32_t cycles,
    const char* tracefile,
    uint32_t latencyRom,
    uint32_t romSize,
    uint32_t blockSize,
    uint32_t* romContent,
    uint32_t numRequests,
    struct Request* requests
){
    sc_clock clk("clk", 1, SC_NS);
    sc_signal<uint32_t> addr("addr"), wdata("wdata"), rdata("rdata");
    sc_signal<uint32_t> mem_addr("mem_addr"), mem_wdata("mem_wdata"), mem_rdata("mem_rdata");
    sc_signal<bool> r("r"), w("w"), wide("w"), ready("ready"), error("error"), mem_r("mem_r"), mem_w("mem_w"), mem_ready("mem_ready");
    sc_signal<uint8_t> user("user");

    MEMORY_CONTROLLER controller("controller", latencyRom, romSize, blockSize, romContent);
    controller.clk(clk);
    controller.addr(addr);
    controller.wdata(wdata);
    controller.r(r);
    controller.w(w);
    controller.wide(wide);
    controller.ready(ready);
    controller.user(user);
    controller.rdata(rdata);
    controller.error(error);
    controller.mem_addr(mem_addr);
    controller.mem_wdata(mem_wdata);
    controller.mem_r(mem_r);
    controller.mem_w(mem_w);
    controller.mem_ready(mem_ready);
    controller.mem_rdata(mem_rdata);

    MAIN_MEMORY memory("main_memory");
    memory.clk(clk);
    memory.addr(mem_addr);
    memory.wdata(mem_wdata);
    memory.r(mem_r);
    memory.w(mem_w);
    memory.rdata(mem_rdata);
    memory.ready(mem_ready);

    sc_trace_file* trace = NULL;
    if(tracefile != NULL){
        trace = sc_create_vcd_trace_file(tracefile);
        sc_trace(trace,clk,"clk");
        sc_trace(trace, addr, "addr");
        sc_trace(trace, wdata, "wdata");
        sc_trace(trace, rdata,  "rdata");
        sc_trace(trace, r, "r");
        sc_trace(trace, w, "w");
        sc_trace(trace, wide, "wide");
        sc_trace(trace, ready, "ready");
        sc_trace(trace, error, "error");
        sc_trace(trace, mem_addr, "mem_addr");
        sc_trace(trace, mem_wdata, "mem_wdata");
        sc_trace(trace, mem_rdata, "mem_rdata");
        sc_trace(trace, mem_r, "mem_r");
        sc_trace(trace, mem_w, "mem_w");
        sc_trace(trace, mem_ready, "mem_ready");
    }

    struct Result res;
    res.cycles = 0;
    res.errors = 0;
    uint32_t usedCycles = 0;
    bool finished = false;

    for(uint32_t i = 0; i < numRequests && !finished; i++){
        addr.write(requests[i].addr);
        wdata.write(requests[i].data);
        w.write(requests[i].w);
        r.write(!requests[i].w);
        wide.write(requests[i].wide);
        user.write(requests[i].user);

        while(!ready.read()){
            if(usedCycles >= cycles){
                finished = true;
                break;
            }
            sc_start(1, SC_NS);
            usedCycles++;
        }
        if(finished){
            break;
        }
        if(error.read()){
            res.errors++;
        } else if(!requests[i].w){
            requests[i].data = rdata.read();
        }
        r.write(false);
        w.write(false);
        if(usedCycles >= cycles){
            finished = true;
        } else{
            sc_start(1, SC_NS);
            usedCycles++;
        }
    }

    res.cycles = usedCycles;
    if(trace != NULL){
        sc_close_vcd_trace_file(trace);
    }
    sc_stop();
    return res;
}

int sc_main(int argc, char *argv[]){
    //TODO: Implement the memory controller and
    Parameters parameters = parse_cli(argc, argv);
    Result res = runSimulation(parameters.cycles, parameters.tracefile, parameters.latencyRom, parameters.romSize, parameters.blockSize, parameters.romContent, parameters.numRequests, parameters.requests);
    printf("Simulation finished after %u cycles with %u access errors.\n", res.cycles, res.errors);
    free(parameters.romContent);
    free(parameters.requests);
    return 0;
}

