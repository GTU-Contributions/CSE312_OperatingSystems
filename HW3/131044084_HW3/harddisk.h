// Mehmed Mustafa 131044084
// 26/05/2017

#ifndef H_HARDDISK
#define H_HARDDISK

#include "memoryBase.h"

class harddisk: public MemoryBase {
    friend class GTUOS;
    public:
        harddisk(){
            hard = (uint8_t*) malloc(HARD_DISK_SIZE);
            for(unsigned i = 0; i<HARD_DISK_SIZE; ++i){
                hard[i] = 0;
            }
        }
        ~harddisk(){free(hard);}
        virtual uint8_t & at(uint32_t index);
        virtual uint8_t & physicalAt(uint32_t index);

    public:
        uint8_t * hard;
};

#endif