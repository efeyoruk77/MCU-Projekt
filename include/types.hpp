#ifndef TYPES_HPP
#define TYPES_HPP

#include <stdint.h>

struct Result{
    uint32_t cycles;
    uint32_t errors;
};

struct Request{
    uint32_t addr;
    uint32_t data;
    uint8_t w;
    uint8_t user;
    uint8_t wide;
};

struct Parameters{
    uint32_t cycles;
    const char* tracefile;
    uint32_t latencyRom;
    uint32_t romSize;
    uint32_t blockSize;
    uint32_t* romContent;
    uint32_t numRequests;
    struct Request* requests;
};

#ifdef  __cplusplus
extern "C" {
#endif

struct Parameters parse_cli(int argc, char** argv);
uint32_t* parseRom(const char* file, uint32_t rom_size);
struct Request* parseRequest(const char* file, uint32_t* numRequests);

#ifdef __cplusplus
}
#endif

#endif