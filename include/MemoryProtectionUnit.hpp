#ifndef MEMORY_PROTECTION_UNIT_H
#define MEMORY_PROTECTION_UNIT_H

#include <cstdint>
#include <unordered_map>

class MemoryProtectionUnit{
private:
uint32_t block_size;
std::unordered_map<uint32_t, uint8_t> owner_map;

public:
MemoryProtectionUnit(uint32_t block_size);
bool accessAndModify(uint32_t address, uint8_t user, bool write, bool wide);
uint8_t getOwner(uint32_t address) const;
};

#endif