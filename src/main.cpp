#include <systemc>
#include <systemc.h>
#include "../include/types.hpp"
#include "cli.c"
using namespace sc_core;

int sc_main(int argc, char *argv[]){
    //TODO: Implement the memory controller and
    std::cout << parse_cli(argc, argv) << std::endl;
    return 0;
}

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
    
}