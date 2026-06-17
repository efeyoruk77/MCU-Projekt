#include "../include/MemoryProtectionUnit.h"

    uint8_t MemoryProtectionUnit::getOwner(uint32_t address) const{
        uint32_t block = address / block_size;
        if(owner_map.count(block) > 0){
            return owner_map.at(block);
        }
        return 255;
    }

    bool MemoryProtectionUnit::accessAndModify(uint32_t address, uint8_t user, bool write, bool wide){
        uint32_t length = wide ? 4 : 1;

        uint32_t start = address / block_size;
        uint32_t end = (address + length -1) / block_size;

        if(user != 0 && user != 255){
            for(int i = start; i <= end; i++){
                if(owner_map.count(i) != 0){
                    if(owner_map.at(i) != user){
                        return false;
                    }   
                }
            }
        }

        for(int i = start; i <= end; i++){
            if(user == 255){
                owner_map.erase(i);
            }
            else if (write)
            {
                owner_map[i] = user;
            }
        }
        return true;
    }