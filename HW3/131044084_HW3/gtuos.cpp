// Mehmed Mustafa 131044084
// 15/04/2017
#include <iostream>
#include <stdio.h>
#include "8080emuCPP.h"
#include "gtuos.h"

// This is just a simple OS with no virtual addresses. 
// You will write your own OS.

using namespace std;

// Initialize static counter for process IDs
// Used to produce automatic Process IDs starting from 20
int GTUOS::Process::processIDcounter = 20;

uint32_t CPBase_GLOBAL;                    // Current Process Base Register
uint32_t CPLimit_GLOBAL = PROCESS_SIZE-1;  // Limit Register
uint32_t MM_CurrBase = STARTING_RAM;       // To know the virtual place of the next created process
uint64_t totalCPUCycles = 0;               // Total Cycles from the CPU

uint64_t GTUOS::handleCall(harddisk & hard, memory & mem, CPU8080 & cpu, unsigned int CRPIndex){

	int value = -9999;
    int VPN, PPN;

    uint64_t cycles = 0;
    uint16_t BCaddress = (cpu.state->b << 8) | cpu.state->c;

	switch(cpu.state->a)
	{
		case 1:
            cout << "System Call: PRINT_B" << endl;
            cout << (int)cpu.state->b << endl;
            cycles = 10;
            break;
		case 2:
            cout << "System Call: PRINT_MEM" << endl;
            cout << (int)cpu.memory->at(BCaddress) << endl;
            cycles = 10;
            break;
		case 3:
            cout << "System Call: READ_B" << endl;
            cin >> value;
            cpu.state->b = (uint8_t)value;
            cout << value << endl;
            cycles = 10;
            break;
		case 4:
            cout << "System Call: READ_MEM" << endl;
            cin >> value;
            cpu.memory->at(BCaddress) = (uint8_t)value;
            cout << value << endl;
            cycles = 10;
            break;
		case 5:
            cout << "System Call: PRINT_STR" << endl;
            cout << &cpu.memory->at(BCaddress) << endl;
            cycles = 100;
            break;
		case 6:
            cout << "System Call: READ_STR" << endl;
            cin >> &cpu.memory->at(BCaddress);
            cycles = 100;
            break;
        case 7:
            cout << "System Call: FORK" << endl;
            if(CRPIndex-1 < 0)
                CRPIndex = pTable.size()-1;

            // Save the modified pages into the HDD
            for(unsigned int i = 0; i < mem.pageTable.size(); ++i){
                VPN = mem.pageTable.at(i)->getVPN();
                PPN = mem.pageTable.at(i)->getPPN();
                mem.store1KBpage(VPN, PPN);
            }

            // Save the PID in Register B
            cpu.state->b = (uint8_t)fork(mem, cpu, CRPIndex);
            cycles = 50;
            break;
        case 8:
            cout << "System Call: EXEC" << endl;
            exec(mem, cpu, CRPIndex);
            cpu.state->pc = 0x0000;
            cycles = 80;
            break;
        case 9:
            cout << "System Call: WAITPID" << endl;
            waitpid(cpu, CRPIndex);
            cycles = 80;
            break;
		default:
            cout << "Error: System call cannot be handled! Value of Register A[";
            cout << (int)cpu.state->a << "] is unexpected!" << endl;
	}

	return cycles;
}
void GTUOS::printPhysicalMemoryToFile(const CPU8080 & cpu){
    FILE *fpt = fopen("-   Memory.mem", "w");
    int newLineCounter = 0;

    // Print the whole physical memory
    fprintf(fpt, "%0004x ", 0);
    for(unsigned int i=0; i<RAM_SIZE; ++i)
    {
        fprintf(fpt, "%02x ", cpu.memory->physicalAt(i));
        ++newLineCounter;
        if(newLineCounter == 16 && i != RAM_SIZE-1){
            newLineCounter = 0;
            fprintf(fpt, "\n%0004x ", i+1);
        }
    }
    fclose(fpt);
}
// Not used in HW3, prints the memory of each process
void GTUOS::printMemoryToFile(unsigned int CRPIndex, const CPU8080 & cpu){
	FILE *filePtr = NULL;
	char memoryFileName[255];
	int newLineCounter = 0;

    char *fileName = (char *)pTable.at(CRPIndex)->getPName();

	// Get the filename part before .com

	memcpy(memoryFileName, fileName, strlen(fileName)-4);
	memoryFileName[strlen(fileName)-4] = '\0';
    sprintf(memoryFileName, "%s_PID[%d]", memoryFileName, pTable.at(CRPIndex)->getPid());
	// Add to the back of it .mem
	strcat(memoryFileName, ".mem");

	filePtr = fopen(memoryFileName, "w");
	if (filePtr == NULL)
	{
		printf("Error: Couldn't open [%s]\n", memoryFileName);
		exit(1);
	}

	fprintf(filePtr, "%0004x ", 0);
	for(unsigned int i=0; i<RAM_SIZE; ++i)
	{
		fprintf(filePtr, "%02x ", cpu.memory->at(i));
		++newLineCounter;
		if(newLineCounter == 16 && i != 0xffff)
        {
			newLineCounter = 0;
			fprintf(filePtr, "\n%0004x ", i+1);
		}
	}

	fclose(filePtr);
}
// ..................................................
void GTUOS::printHardDisk(harddisk & hard){
    FILE *fpt = fopen("-   HardDisk.mem", "w");
    int newLineCounter = 0;
    //uint8_t *buffer = &hard.physicalAt(offset);
    // Print the whole hard disk
    fprintf(fpt, "%0004x ", 0);
    for(unsigned int i=0; i<HARD_DISK_SIZE; ++i)
    {
        fprintf(fpt, "%02x ", hard.physicalAt(i));
        ++newLineCounter;
        if(newLineCounter == 16 && i != (HARD_DISK_SIZE-1)){
            newLineCounter = 0;
            fprintf(fpt, "\n%0004x ", i+1);
        }
    }
    fclose(fpt);
}
void GTUOS::generateRandomNumbers(){

    int firstDigit, secondDigit;
    // Generate random
    FILE *fnum = fopen("-   RandomNumbers.mem", "w");
    srand(time(NULL));
    for(int i=0; i<NUMS_TO_GEN; ++i)
    {
        firstDigit = rand()%16;
        secondDigit = rand()%16;
        if(firstDigit >= 10)
            fprintf(fnum, "0%01x%01xH,", firstDigit, secondDigit);
        else
            fprintf(fnum, "%01x%01xH,", firstDigit, secondDigit);
    }

    fclose(fnum);
}

// Process functions implementation
void GTUOS::ReadProgramIntoHarddisk(harddisk * hard, const char* filename, uint32_t offset){
    int fsize;

    FILE *f= fopen(filename, "rb");
    if(f==NULL)
    {
        printf("Error: Couldn't open [%s]\n", filename);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);
    uint8_t *buffer = &hard->physicalAt(offset);
    fread(buffer, fsize, 1, f);

    fclose(f);
}
void GTUOS::printProcessTable(){
    int longestName = 0;
    for(unsigned int i=0; i<pTable.size(); ++i){
        if(longestName < strlen(pTable.at(i)->getPName()))
            longestName = strlen(pTable.at(i)->getPName());
    }

    printf("--------------------------------PROCESS_TABLE---------------------------------\n");
    cout << setw(longestName+1) << left << "ProcessName" << setw(8) << left << "BaseReg"
         << setw(9)  << left    << "LimitReg"   << setw(6)  << left    << "PID"
         << setw(6)  << left    << "PPID"       << setw(6)  << left    << "WPID"
         << setw(11) << left    << "UsedCycles" << setw(8)  << left    << "Started"
         << setw(9)  << left    << "STATE" << endl;

    for(unsigned int i=0; i<pTable.size(); ++i){
        char state[255];
        switch(pTable.at(i)->getProcessState()){
            case READY:     strcpy(state, "READY");     break;
            case BLOCKED:   strcpy(state, "BLOCKED");   break;
            case RUNNING:   strcpy(state, "RUNNING");   break;
            case FINISHED:  strcpy(state, "FINISHED");  break;
        }

        cout << setw(longestName+1) << left << pTable.at(i)->getPName()
             << setw(8)  << left << pTable.at(i)->getBaseReg() << setw(9)  << left << pTable.at(i)->getLimitReg()
             << setw(6)  << left << pTable.at(i)->getPid()     << setw(6)  << left << pTable.at(i)->getPPid()
             << setw(6)  << left << pTable.at(i)->getWPid()    << setw(11) << left << pTable.at(i)->getUC()
             << setw(8)  << left << pTable.at(i)->getSC()      << setw(9)  << left << state << endl;

    }
    printf("------------------------------------------------------------------------------\n");

}
void GTUOS::createProcess(memory & mem, CPU8080 & cpu, char *pN, int ppid){
    // Fails if no memory left on hard disk
    if(MM_CurrBase >= HARD_DISK_SIZE-1){
        return;
    }
    // Get the static processIDcounter
    int pid = GTUOS::Process::processIDcounter;
    // Create new process
    Process *thisProcess = new Process(pN, *cpu.state, ppid);

    // Push the process in the process table
    pTable.push_back(thisProcess);

    CPBase_GLOBAL = thisProcess->getBaseReg();
    CPLimit_GLOBAL = thisProcess->getLimitReg();

    ReadProgramIntoHarddisk(mem.hard, pN, thisProcess->getBaseReg());

    int VirtualPageNumber = 0;
    //Store the first page of the first program into the Memory
    mem.mapVTP(VirtualPageNumber);

}
void GTUOS::finishProcess(int pTableIndex){
    pTable.at((unsigned int)pTableIndex)->setProcessState(FINISHED);
}
int GTUOS::contentSwitch(CPU8080 & cpu, unsigned int & CRPIndex, int & blockedCycles, int & usedCycles, char *blockedPName, char* nextPName){
    int breakCounter = 0;

    // Save the content of the process which is recently blocked if it's not finished
    if(pTable.at(CRPIndex)->getProcessState() != FINISHED){
        pTable.at(CRPIndex)->setProcessState(BLOCKED);
        pTable.at(CRPIndex)->setRegState(*cpu.state);
        pTable.at(CRPIndex)->addUsedCycles(usedCycles);
        blockedCycles = pTable.at(CRPIndex)->getUC();
        strcpy(blockedPName, pTable.at(CRPIndex)->getPName());
        // If the process doesn't wait for another process then it's READY
        if(pTable.at(CRPIndex)->getWPid() == 0)
            pTable.at(CRPIndex)->setProcessState(READY);
    }

    // Select the next process which will be runned
    ++CRPIndex;
    if(CRPIndex >= pTable.size())
        CRPIndex = CRPIndex % pTable.size();

    // Check processes until finding process in READY state
    while(pTable.at(CRPIndex)->getProcessState() != READY){
        ++CRPIndex;
        if(CRPIndex >= pTable.size())
            CRPIndex = CRPIndex % pTable.size();

        // If no Ready process is available break
        ++breakCounter;
        if(breakCounter > pTable.size()+1)
            return -1;
    }

    // Load the content of the next process which will be runned
    //*cpu.state = pTable.at(CRPIndex)->getRegState();

    cpu.state->a = pTable.at(CRPIndex)->getRegState().a;
    cpu.state->b = pTable.at(CRPIndex)->getRegState().b;
    cpu.state->c = pTable.at(CRPIndex)->getRegState().c;
    cpu.state->d = pTable.at(CRPIndex)->getRegState().d;
    cpu.state->e = pTable.at(CRPIndex)->getRegState().e;
    cpu.state->h = pTable.at(CRPIndex)->getRegState().h;
    cpu.state->l = pTable.at(CRPIndex)->getRegState().l;
    cpu.state->sp = pTable.at(CRPIndex)->getRegState().sp;
    cpu.state->pc = pTable.at(CRPIndex)->getRegState().pc;
    cpu.state->cc = pTable.at(CRPIndex)->getRegState().cc;

    CPBase_GLOBAL = pTable.at(CRPIndex)->getBaseReg();
    CPLimit_GLOBAL = pTable.at(CRPIndex)->getLimitReg();
    strcpy(nextPName, pTable.at(CRPIndex)->getPName());

    // Set the process state to RUNNING
    pTable.at(CRPIndex)->setProcessState(RUNNING);

    usedCycles = 0;
    return CRPIndex;
}

int GTUOS::fork(memory & mem, CPU8080 & cpu, unsigned int CRPIndex){
    // Fails if no memory left
    if(MM_CurrBase >= HARD_DISK_SIZE-1){
        return 1;
    }

    int pid = GTUOS::Process::processIDcounter;
    int ppid = pTable.at(CRPIndex)->getPid();
    char *pN = (char *)pTable.at(CRPIndex)->getPName();

    Process *childProcess = new Process(pN, *cpu.state, ppid);

    uint32_t parentBase = pTable.at(CRPIndex)->getBaseReg();
    uint32_t childBase = childProcess->getBaseReg();
    uint8_t value;


    // Copy the memory content of the parent to the child
    for(uint32_t index = 0; index <= PROCESS_SIZE-1; ++index){
        CPBase_GLOBAL = parentBase;
        value = mem.hard->at(index);
        CPBase_GLOBAL = childBase;
        mem.hard->at(index) = value;
    }

    CPBase_GLOBAL = parentBase;

    // Push the child into the Process Table
    pTable.push_back(childProcess);
    // Return the child process id
    return pid;
}
int GTUOS::exec(memory & mem, CPU8080 & cpu, unsigned int CRPIndex){
    uint16_t BCaddress = (cpu.state->b << 8) | cpu.state->c;
    char *fileName = (char*)&cpu.memory->at(BCaddress);
    char fileNameOrg[255];
    FILE *fptr = fopen(fileName, "rb");
    if(fptr == NULL){
        printf("EXEC ERROR: Couldn't open [%s]\n", fileName);
        return -1;
    }

    strcpy(fileNameOrg, fileName);

    fseek(fptr, 0L, SEEK_END);
    int fsize = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    CPBase_GLOBAL = pTable.at(CRPIndex-1)->getBaseReg();
    // Read the new program into Hard disk
    uint8_t *buffer = &mem.hard->physicalAt(pTable.at(CRPIndex)->getBaseReg());
    fread(buffer, fsize, 1, fptr);

    mem.printPageTable();

    int VPN, PPN;
    int sVPN = memory::calculatePageNumber((int)pTable.at(CRPIndex)->getBaseReg());
    int eVPN = sVPN + memory::calculatePageNumber(PROCESS_SIZE);

    // Update the RAM with the new program in this process
    for(unsigned int i=0; i<mem.pageTable.size(); ++i){
        VPN = mem.pageTable.at(i)->getVPN();
        PPN = mem.pageTable.at(i)->getPPN();
        if(VPN >= sVPN && VPN < eVPN){
            mem.load1KBpage(VPN, PPN);
        }
    }

    pTable.at(CRPIndex)->setPC(0x0000);
    pTable.at(CRPIndex)->setPName(fileNameOrg);
    //cout << "FileNameOrg[" << pTable.at(CRPIndex)->getPName() << "]" << endl;

    fclose(fptr);
    return 0;
}
int GTUOS::waitpid(CPU8080 & cpu, unsigned int CRPIndex){
    pTable.at(CRPIndex)->setWPid(cpu.state->b);

    return 0;
}
