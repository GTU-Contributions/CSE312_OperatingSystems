// Mehmed Mustafa 131044084
// 26/05/2017

#include <iostream>
#include "8080emuCPP.h"
#include "harddisk.h"

using namespace std;

// Returns virtual (logical) address at index
uint8_t & harddisk::at(uint32_t index){
    uint32_t virtualAddress;
    uint32_t endingAddress = CPBase_GLOBAL + CPLimit_GLOBAL;

    // Calculate the virtual address
    virtualAddress = CPBase_GLOBAL + index;

    // Check if the accessed location is beyond the limit
    if(virtualAddress > endingAddress){
        cout << "Segmentation fault HardDisk!!! VirtualAddress[" << virtualAddress << "] & Index["  << index << "]" << endl;
        cout << "Starting Addr[" << CPBase_GLOBAL << "] & Ending Addr[" << endingAddress << "]" << endl;
        exit(1);
    }

    return hard[virtualAddress];
}

// Returns physical address at index
uint8_t & harddisk::physicalAt(uint32_t index){
    return hard[index];
}