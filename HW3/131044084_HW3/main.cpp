// Mehmed Mustafa 131044084
// 15/04/2017
#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;
#define QUANTUM_TIME 100 // Round Robin Scheduler Quantum Time in cycles


int main (int argc, char**argv)
{
    /*
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1); 
	}
    */

	//int DEBUG = atoi(argv[2]);
    int DEBUG = 0;

    harddisk hard;
    memory mem(&hard);
	CPU8080 theCPU(&mem);
    GTUOS	theOS;

    // Generate random 10 000 numbers
    // theOS.generateRandomNumbers();

    // Create the processes
    //theOS.createProcess(mem, theCPU, argv[1], 0);
    theOS.createProcess(mem, theCPU, (char *)"loadsort.com", 0);

    unsigned int CRPIndex = 0;  // Current Running Process Index
    int blockedCycles = 0;      // Total used cycles by the blocked process
    int usedCycles = 0;         // Used cycles by the current running process

    char blockedPName[255];     // Name of the blocked process
    char nextPName[255];        // Name of the next process

    // Round Robin Scheduler implemented here
    // Loop while there are READY processes
    while(theOS.contentSwitch(theCPU, CRPIndex, blockedCycles, usedCycles, blockedPName, nextPName) != -1){
        // Loop while the used cycles by the current process are less than the QUANTUM_TIME
        while(usedCycles < QUANTUM_TIME) {
            usedCycles += theCPU.Emulate8080p(DEBUG);

            if (theCPU.isSystemCall())
                usedCycles += theOS.handleCall(hard, mem, theCPU, CRPIndex);

            if(DEBUG == 1){
                // Print the page table after every instruction
                mem.printPageTable();
            }

            if(DEBUG == 2){
                // Printf the page table after every instruction and wait for an input to continue
                mem.printPageTable();
                cin.get();
            }

            if(DEBUG == 3){
                if(PAGE_FAULT){
                    cout << "PAGE FAULT OCCURED!!" << endl;
                    mem.printPageTable();
                    if(PAGES_REPLACED){
                        printf("VirtualPage: %d at PhysicalPage: %d was replaced by the\n", oldVPN, oldPPN);
                        printf("VirtualPage: %d currently at PhysicalPage: %d\n", newVPN, newPPN);
                        PAGES_REPLACED = false;
                    }
                    PAGE_FAULT = false;
                }
            }

            // If the program of some process is ended BLOCK that process
            if(theCPU.isHalted()){
                theOS.finishProcess(CRPIndex);
                break;
            }
        };

        totalCPUCycles += usedCycles;
    };

    if(DEBUG == 0 || DEBUG == 1){
        // Print the Physical Memory to file
        theOS.printPhysicalMemoryToFile(theCPU);
        // Print the Hard Disk to file
        theOS.printHardDisk(hard);
    }

    // Print statistics
    cout << "Pages Replaced: " << pagesReplaced << endl;
    cout << "Page Faults: " << pageFaults << endl;

    return 0;
}