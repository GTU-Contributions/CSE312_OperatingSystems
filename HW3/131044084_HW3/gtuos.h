// Mehmed Mustafa 131044084
// 15/04/2017
#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"
#include "harddisk.h"
#include "memory.h"
#include <string.h>
#include <iomanip>
#include <vector>
#include <time.h>

using namespace std;

#define NUMS_TO_GEN  10000 // Number of the numbers to be generated randomly
#define PROCESS_SIZE 0x4000 // 16KB
#define STARTING_RAM 0x0000 // Start from 0x0000

typedef enum ProcessStates{ READY, BLOCKED, RUNNING, FINISHED } ProcessStates;

class GTUOS{
    friend class memory;
	public:
        class Process{
            public:
                Process(char * pN, State8080 rS, int pp){
                    strcpy(processName, pN);
                    setRegState(rS);
                    this->baseReg = MM_CurrBase;
                    this->limitReg = CPLimit_GLOBAL;
                    this->pid = processIDcounter;
                    this->ppid = pp;
                    this->wpid = 0;
                    this->SC = totalCPUCycles;
                    this->UC = 0;
                    this->pState = READY;

                    ++processIDcounter;
                    MM_CurrBase += PROCESS_SIZE;
                }

                // Other useful functions
                int addUsedCycles(int cycles){ this->UC += cycles; }

                // Setters only those which are needed
                void setRegState(State8080 rState){
                    //this->rState = rState;
                    this->rState.a = rState.a;
                    this->rState.b = rState.b;
                    this->rState.c = rState.c;
                    this->rState.d = rState.d;
                    this->rState.e = rState.e;
                    this->rState.h = rState.h;
                    this->rState.l = rState.l;
                    this->rState.sp = rState.sp;
                    this->rState.pc = rState.pc;

                    this->rState.cc = rState.cc;
                }
                void setProcessState(ProcessStates pS){ this->pState = pS; }
                void setPC(uint16_t pc){ this->rState.pc = pc;}
                void setWPid(int wpid){ this->wpid; }
                void setPName(char* name){
                    strncpy(processName, name, strlen(name)+1);
                }

                // Getters
                const char* getPName(){ return processName; }
                State8080 getRegState(){ return rState; }
                int getPid(){ return pid; }
                int getPPid(){ return ppid; }
                int getWPid(){ return wpid; }
                uint32_t getBaseReg(){ return baseReg; }
                uint32_t getLimitReg(){ return limitReg; }
                uint64_t getSC(){ return SC; }
                int getUC(){ return UC; }
                ProcessStates getProcessState(){ return this->pState;}

                static int processIDcounter;    // To produce automatic Process ID's

                State8080 rState;         // Saved register set of 8080 chip
            private:
                char processName[255];
                uint32_t baseReg;
                uint32_t limitReg;
                int pid;                    // process id
                int ppid;                   // parent pid
                int wpid;                   // waiting for pid
                uint64_t SC;                // Starting time of the process(the cycle number of the CPU)
                int UC;                     // How many cycles the process has used so far
                ProcessStates pState;       // The state of the process (Ready, Blocked, Running)
        };

    uint64_t handleCall(harddisk & hard, memory & mem, CPU8080 & cpu, unsigned int CRPIndex);
    void printPhysicalMemoryToFile(const CPU8080 & cpu);
    void printMemoryToFile(unsigned int CRPIndex, const CPU8080 & cpu);
    void printHardDisk(harddisk & hard);
    void generateRandomNumbers();

    // Process functions
    void ReadProgramIntoHarddisk(harddisk * hard, const char* filename, uint32_t offset);
    void printProcessTable();
    void createProcess(memory & mem, CPU8080 & cpu, char *pN, int ppid);
    void finishProcess(int pTableIndex);
    int contentSwitch(CPU8080 & cpu, unsigned int & CRPIndex, int & blockedCycles, int & usedCycles, char *blockedPName, char* nextPName);
    int fork(memory & mem, CPU8080 & cpu, unsigned int CRPIndex);
    int exec(memory & mem, CPU8080 & cpu, unsigned int CRPIndex);
    int waitpid(CPU8080 & cpu, unsigned int CRPIndex);

    private:
        vector<Process*> pTable;   // Process Table
};

#endif