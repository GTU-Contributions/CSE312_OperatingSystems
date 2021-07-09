// Mehmed Mustafa 131044084
// 15/04/2017
#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"

using namespace std;
#define QUANTUM_TIME 100 // Round Robin Scheduler Quantum Time in cycles

	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{
    /*
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1); 
	}
    */

	//int DEBUG = atoi(argv[2]);
    int DEBUG = 3;

    memory mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS;

    //theOS.createProcess(theCPU, argv[1], 0x0000, 0x4000);

    // Create the processes
    theOS.createProcess(theCPU, (char *)"parallel.com", 0);
    //theOS.createProcess(theCPU, (char *)"printNumbersRev.com", 0);
    //theOS.fork(theCPU, 0);

    unsigned int CRPIndex = 0;  // Current Running Process Index
    int blockedCycles = 0;      // Total used cycles by the blocked process
    int usedCycles = 0;         // Used cycles by the current running process

    char blockedPName[255];     // Name of the blocked process
    char nextPName[255];        // Name of the next process

    // Round Robin Scheduler implemented here
    // Loop while there are READY processes
    while(theOS.contentSwitch(theCPU, CRPIndex, blockedCycles, usedCycles, blockedPName, nextPName) != -1){
        // Loop while the used cycles by the current process are less than the QUANTUM_TIME
        while(usedCycles < QUANTUM_TIME+10) {
            usedCycles += theCPU.Emulate8080p(DEBUG);

            if (theCPU.isSystemCall())
                usedCycles += theOS.handleCall(theCPU, CRPIndex);

            // If the program of some process is ended BLOCK that process
            if(theCPU.isHalted()){
                theOS.finishProcess(CRPIndex);
                // Save the memory content
                if(DEBUG == 1 || DEBUG == 0)
                    theOS.printMemoryToFile(CRPIndex, theCPU);
                break;
            }
        };

        totalCPUCycles += usedCycles;

        // Emulate8080p de ki debug kontrolunden dolayi CPU State de basıyor ekrana DEBUG 2 olursa !!!
        if(DEBUG == 2){
            printf("Content Switch from %s to %s\n", blockedPName, nextPName);
            cout << "Cycles used by the blocked process: " << blockedCycles << endl;
        }

        if(DEBUG == 3){
            theOS.printProcessTable();
        }
    };

    theOS.printPhysicalMemoryToFile(theCPU);

    return 0;

}