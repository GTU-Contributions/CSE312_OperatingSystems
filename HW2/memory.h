// Mehmed Mustafa 131044084
// 15/04/2017
#ifndef H_MEMORY
#define H_MEMORY

#include "memoryBase.h"
#include "gtuos.h"

// This is just a simple memory with no virtual addresses. 
// You will write your own memory with base and limit registers.

class memory: public MemoryBase {
    friend class GTUOS;
    public:
        memory(){mem = (uint8_t*) malloc(0x10000);}
		~memory(){ free(mem);}
		virtual uint8_t & at(uint32_t index);
		virtual uint8_t & physicalAt(uint32_t index);

    private:
		uint8_t * mem;
};

#endif


