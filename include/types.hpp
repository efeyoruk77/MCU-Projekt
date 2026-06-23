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

#endif