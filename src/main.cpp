#include <systemc>
#include <systemc.h>
#include "../include/types.hpp"
#include "cli.c"
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
    Result res;
    //TODO
    return res;
}

int sc_main(int argc, char *argv[]){
    //TODO: Implement the memory controller and
    Parameters parameters = parse_cli(argc, argv);
    Result res = runSimulation(parameters.cycles, parameters.tracefile, parameters.latencyRom, parameters.romSize, parameters.blockSize, parameters.romContent, parameters.numRequests, parameters.requests);
    return 0;
}

