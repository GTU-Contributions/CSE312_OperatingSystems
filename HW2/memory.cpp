// Mehmed Mustafa 131044084
// 15/04/2017

#include <iostream>
#include "8080emuCPP.h"
#include "memory.h"

using namespace std;

// Returns virtual (logical) address at index
uint8_t & memory::at(uint32_t index){
    uint32_t virtualAddress;
    uint32_t sum = CPBase_GLOBAL + CPLimit_GLOBAL;

    // Calculate the virtual address
    virtualAddress = CPBase_GLOBAL + index;

    // Check if the accessed location is beyond the limit
    if(virtualAddress > sum){
        cout << "Segmentation fault!!! VirtualAddress[" << virtualAddress << "] & Index["  << index << "]" << endl;
        exit(1);
    }

    return mem[virtualAddress];
}

// Returns physical address at index
uint8_t & memory::physicalAt(uint32_t index){
    return mem[index];
}