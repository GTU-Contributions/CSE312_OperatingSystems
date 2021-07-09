// Mehmed Mustafa 131044084
// 15/04/2017
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include "8080emuCPP.h"
#include "gtuos.h"

// This is just a simple OS with no virtual addresses. 
// You will write your own OS.

using namespace std;

// Initialize static counter for process IDs
// Used to produce automatic Process IDs starting from 20
int GTUOS::Process::processIDcounter = 20;

uint32_t CPBase_GLOBAL;            // Current Process Base Register
uint32_t CPLimit_GLOBAL = 0x3FFF;  // Limit Register
uint32_t MM_CurrBase = 0x0000;     // To know the virtual place of the next created process
uint64_t totalCPUCycles = 0;       // Total Cycles from the CPU

GTUOS::Process::Process(char * pN, State8080 rS, int pp)
{
    strcpy(processName, pN);
    this->rState = rS;
    this->baseReg = MM_CurrBase;
    this->limitReg = CPLimit_GLOBAL;
    this->pid = processIDcounter;
    this->ppid = pp;
    this->wpid = 0;
    this->SC = totalCPUCycles;
    this->UC = 0;
    this->pState = READY;

    ++processIDcounter;
    MM_CurrBase += 0x4000;
}

uint64_t GTUOS::handleCall(CPU8080 & cpu, unsigned int CRPIndex){

	int value = -9999;

    uint64_t cycles = 0;
    uint16_t BCaddress = (cpu.state->b << 8) | cpu.state->c;

	switch(cpu.state->a)
	{
		case 1:
			cout << "System Call: PRINT_B" << endl;
			cout << "Value of B " << (int)cpu.state->b << endl;
			cycles = 10;
			break;
		case 2:
			cout << "System Call: PRINT_MEM" << endl;
			cout << "Value of MEM[BC] " << (int)cpu.memory->at(BCaddress) << endl;
			cycles = 10;
			break;
		case 3:
			cout << "System Call: READ_B" << endl;
			cin >> value;
			cpu.state->b = (uint8_t)value;
			cout << "Readed " << value << " into B" << endl;
			cycles = 10;
			break;
		case 4:
			cout << "System Call: READ_MEM" << endl;
			cin >> value;
			cpu.memory->at(BCaddress) = (uint8_t)value;
			cout << "Readed " << value << " into MEM[BC]" << endl;
			cycles = 10;
			break;
		case 5:
			cout << "System Call: PRINT_STR" << endl;
			cout << "String: ";
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

            cpu.state->b = (uint8_t)fork(cpu, CRPIndex);
            cycles = 50;
            break;
        case 8:
            cout << "System Call: EXEC" << endl;
            exec(cpu, CRPIndex);
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
    FILE *fpt = fopen("AllMemory.mem", "w");
    int newLineCounter = 0;
    // Print the whole physical memory
    fprintf(fpt, "%0004x ", 0);
    for(unsigned int i=0; i<0x10000; ++i)
    {
        fprintf(fpt, "%02x ", cpu.memory->physicalAt(i));
        ++newLineCounter;
        if(newLineCounter == 16 && i != 0xffff){
            newLineCounter = 0;
            fprintf(fpt, "\n%0004x ", i+1);
        }
    }
}
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
	for(unsigned int i=0; i<0x4000; ++i)
	{
		fprintf(filePtr, "%02x ", cpu.memory->at(i));
		++newLineCounter;
		if(newLineCounter == 16 && i != 0xffff){
			newLineCounter = 0;
			fprintf(filePtr, "\n%0004x ", i+1);
		}
	}

	fclose(filePtr);
}
void GTUOS::printProcessTable(){
    int longestName = 0;
    for(unsigned int i=0; i<pTable.size(); ++i){
        if(longestName < strlen(pTable.at(i)->getPName()))
            longestName = strlen(pTable.at(i)->getPName());
    }

    printf("------------------------------------------------------------------------------\n");
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
void GTUOS::createProcess(CPU8080 & cpu, char *pN, int ppid){
    // Fails if no memory left
    if(MM_CurrBase >= 0xFFFF){
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

    cpu.ReadFileIntoMemoryAt(pN, thisProcess->getBaseReg());
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

        ++breakCounter;
        if(breakCounter > pTable.size()+1)
            return -1;
    }

    // Load the content of the next process which will be runned
    *cpu.state = pTable.at(CRPIndex)->getRegState();
    CPBase_GLOBAL = pTable.at(CRPIndex)->getBaseReg();
    CPLimit_GLOBAL = pTable.at(CRPIndex)->getLimitReg();
    strcpy(nextPName, pTable.at(CRPIndex)->getPName());
    // Set the process state to RUNNING
    pTable.at(CRPIndex)->setProcessState(RUNNING);

    usedCycles = 0;
    return CRPIndex;
}

int GTUOS::fork(CPU8080 & cpu, unsigned int CRPIndex){
    // Fails if no memory left
    if(MM_CurrBase >= 0xFFFF){
        return 1;
    }

    int pid = GTUOS::Process::processIDcounter;
    int ppid = pTable.at(CRPIndex)->getPid();
    char *pN = (char *)pTable.at(CRPIndex)->getPName();
    State8080 state = pTable.at(CRPIndex)->getRegState();
    //uint32_t limitReg = pTable.at(CRPIndex)->getLimitReg();

    Process *childProcess = new Process(pN, state, ppid);

    uint32_t parentBase = pTable.at(CRPIndex)->getBaseReg();
    //uint32_t childBase = childProcess->getBaseReg();

    //uint8_t value;

    /*
    cout << "CRPIndex: " << CRPIndex << endl;
    cout << "ParentBase: " << parentBase << endl;
    // Copy the memory content of the parent to the child
    for(uint32_t index = parentBase; index <= (parentBase+limitReg); ++index){
        CPBase_GLOBAL = parentBase;
        value = cpu.memory->at(index);
        CPBase_GLOBAL = childBase;
        cpu.memory->at(index) = value;
    }
    */

    CPBase_GLOBAL = parentBase;

    pTable.push_back(childProcess);
    return pid;
}
int GTUOS::exec(CPU8080 & cpu, unsigned int CRPIndex){
    int pid = cpu.state->d; // If any pid is saved in Register D take it
    uint16_t BCaddress = (cpu.state->b << 8) | cpu.state->c;
    char *fileName = (char*)&cpu.memory->at(BCaddress);
    FILE *fptr = fopen(fileName, "rb");
    if(fptr == NULL){
        printf("EXEC ERROR: Couldn't open [%s]\n", fileName);
        return -1;
    }

    fseek(fptr, 0L, SEEK_END);
    int fsize = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    int pidIndex = -1;
    for(unsigned int i=0; i<pTable.size(); ++i)
        if(pTable.at(i)->getPid() == pid)
            pidIndex = i;

    if(pidIndex != -1){
        CPBase_GLOBAL = pTable.at(pidIndex)->getBaseReg();
        uint8_t *buffer = &cpu.memory->physicalAt(pTable.at(pidIndex)->getBaseReg());
        fread(buffer, fsize, 1, fptr);
        pTable.at(pidIndex)->setPName(fileName);
        pTable.at(pidIndex)->setPC(0x0000);
    }
    else{
        uint8_t *buffer = &cpu.memory->physicalAt(pTable.at(CRPIndex)->getBaseReg());
        fread(buffer, fsize, 1, fptr);
        pTable.at(CRPIndex)->setPName(fileName);
        pTable.at(CRPIndex)->setPC(0x0000);
    }

    fclose(fptr);

    return 0;
}
int GTUOS::waitpid(CPU8080 & cpu, unsigned int CRPIndex){
    pTable.at(CRPIndex)->setWPid(cpu.state->b);

    return 0;
}