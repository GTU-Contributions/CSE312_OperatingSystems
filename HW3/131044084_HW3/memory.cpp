// Mehmed Mustafa 131044084
// 15/04/2017

#include <iostream>
#include "8080emuCPP.h"
#include "memory.h"
#include "gtuos.h"

using namespace std;

uint64_t pagesReplaced = 0;  // Holds the total number of the replaced pages
uint64_t pageFaults = 0;     // Holds the total number of page faults
bool PAGE_FAULT = false;
bool PAGES_REPLACED = false;

int oldVPN = 0, oldPPN = 0, newVPN = 0, newPPN = 0;

// Returns logical address at index
uint8_t & memory::at(uint32_t index){
    int virtualPageAddress = (int)CPBase_GLOBAL + (int)index;
    int VPN = calculatePageNumber(virtualPageAddress);
    int PPN = isPagePresent(VPN);
    int physicalPageAddress = 0, offset = 0;

    offset = virtualPageAddress - VPN*PAGE_SIZE;
    physicalPageAddress = PPN*1024 + offset;

    if(PPN == NOT_IN_RAM){
        ++pageFaults;
        PAGE_FAULT = true;
        mapVTP(VPN);

        virtualPageAddress = (int)CPBase_GLOBAL + (int)index;
        VPN = calculatePageNumber(virtualPageAddress);
        offset = virtualPageAddress - VPN*PAGE_SIZE;
        PPN = isPagePresent(VPN);
        physicalPageAddress = PPN*1024 + offset;
    }
    else{
        offset = virtualPageAddress - VPN*PAGE_SIZE;
        physicalPageAddress = PPN*1024 + offset;
    }

    return mem[physicalPageAddress];
}

// Returns physical address at index
uint8_t & memory::physicalAt(uint32_t index){
    return mem[index];
}

// Prints only pages which are presented in the memory
void memory::printPageTable(){
    printf("---------------------------------PAGE_TABLE-----------------------------------\n");
    cout << setw(2) << left << "P" << setw(2) << left << "M"
         << setw(4) << left << "VPN" << setw(15) << left << "VirtualAddress"
         << setw(4) << left << "PPN" << setw(16) << left << "PhysicalAddress" << endl;

    int VPN, PPN;
    bool present, modified;
    for(unsigned int i=0; i<pageTable.size(); ++i){
        present = pageTable.at(i)->getPresent();
        // If the page is not in the memory, don't print
        if(!present)
            continue;
        modified = pageTable.at(i)->getModified();
        VPN = pageTable.at(i)->getVPN();
        PPN = pageTable.at(i)->getPPN();

        cout << setw(2)  << left << present << setw(2) << left << modified
             << setw(4)  << left << VPN;
        printf("%0004x-%0004x      ", calculateSA(VPN), calculateEA(VPN));

        if(PPN == NOT_IN_RAM){
            printf("%d  NULL", NOT_IN_RAM);
        }
        else{
            cout << setw(4) << left << PPN;
            printf("%0004x-%0004x", calculateSA(PPN), calculateEA(PPN));
        }
        cout << endl;
    }
    printf("------------------------------------------------------------------------------\n");

}
// Check's if the Page is in the Memory if so
// returns the Physical Page Number, else returns NOT_IN_RAM(-1)
int memory::isPagePresent(int VPN){
    for(unsigned i = 0; i<pageTable.size(); ++i){
        if(pageTable.at(i)->getVPN() == VPN) {
            return pageTable.at(i)->getPPN();
        }
    }

    return NOT_IN_RAM;
}

// Load 1KB page into Memory
void memory::load1KBpage(int VPN, int PPN){
    // Value of the cell to be copied
    uint8_t value;
    // Calculate starting addresses of Virtual Page Number and Physical Page Number
    int SA_VPN = memory::calculateSA(VPN);
    int SA_PPN = memory::calculateSA(PPN);

    for(uint32_t offset = 0; offset <= PAGE_SIZE-1; ++offset){
        value = hard->physicalAt(SA_VPN + offset);
        mem[SA_PPN + offset] = value;
    }
}
// Store 1KB page Into Hard Disk
void memory::store1KBpage(int VPN, int PPN){
    // Value of the cell to be copied
    uint8_t value;
    // Calculate starting addresses of Virtual Page Number and Physical Page Number
    int SA_VPN = memory::calculateSA(VPN);
    int SA_PPN = memory::calculateSA(PPN);

    for(uint32_t offset = 0; offset <= PAGE_SIZE-1; ++offset){
        value = mem[SA_PPN + offset];
        hard->physicalAt(SA_VPN + offset) = value;
    }
}
// Map Virtual to Physical Page FIFO Algorithm
void memory::mapVTP(int VPN){
    int wVPN = 0, wPPN = 0;
    bool modified = false;
    int PPN = 0;

    // RAM_SIZE/PAGE_SIZE is 8 for this homework
    // If there is a free page in the table use it
    if(pageTable.size() < 8){
        PPN = pageTable.size();
        load1KBpage(VPN, PPN);
        pageTable.push_back(new Page(VPN, PPN, true, true));
    }
        // No free space in the page table, so delete the oldest page reference and add new
    else{
        // Keep the values of the page which will be deleted
        modified = pageTable.at(0)->getModified();
        wVPN = pageTable.at(0)->getVPN();
        oldVPN = wVPN;

        // Save the freed Physical Page Number to use it to store the new Virtual Page Number
        wPPN = pageTable.at(0)->getPPN();
        oldPPN = wPPN;

        // If the page to be deleted was modified, write it back to the Hard Disk
        if(modified){
            store1KBpage(wVPN, wPPN);
        }
        // Delete the oldest page reference(which is always the first page since we use FIFO algorithm)
        // and shift all elements in the vector to the left by 1
        for(unsigned i=0; i<pageTable.size()-1; ++i){
            pageTable.at(i) = pageTable.at(i+1);
        }

        // Delete the last element since it is copied on the left
        pageTable.pop_back();
        load1KBpage(VPN, wPPN);
        newVPN = VPN;
        newPPN = wPPN;

        ++pagesReplaced;
        PAGES_REPLACED = true;
        // Actually we set the every next element always on RAM_SIZE/PAGE_SIZE-1,
        // index 7 for this homework since this is the tail of the pageTable
        pageTable.push_back(new Page(VPN, wPPN, true, true));
    }
}
