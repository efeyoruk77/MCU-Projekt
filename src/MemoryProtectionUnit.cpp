#include "../include/MemoryProtectionUnit.hpp"

    sc_uint<8> MemoryProtectionUnit::getOwner(uint32_t address) const{
        uint32_t block = address / block_size;
        if(owner_map.count(block) > 0){
            return owner_map.at(block);
        }
        return 255;
    }

    bool MemoryProtectionUnit::accessAndModify(uint32_t address, sc_uint<8> user, bool write, bool wide){
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

    void MemoryProtectionUnit::checkAccess(){
        uint32_t address = input_address.read();
        sc_uint<8> user = input_user.read();
        bool write = input_write.read();
        bool wide = input_wide.read();

        bool allowed = accessAndModify(address, user, write, wide);

        output_allowed.write(allowed);
        output_owner.write(getOwner(address));
    }