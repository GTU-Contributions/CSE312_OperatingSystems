// Mehmed Mustafa 131044084
// 15/04/2017
#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"
#include <string.h>
#include <vector>

typedef enum ProcessStates{ READY, BLOCKED, RUNNING, FINISHED } ProcessStates;

using namespace std;

extern uint32_t CPBase_GLOBAL;  // Current Process Base Register
extern uint32_t CPLimit_GLOBAL; // Current Process Limit Register
extern uint32_t MM_CurrBase;    // To know the virtual place of the next created process
extern uint64_t totalCPUCycles; // Total Cycles from the CPU

class GTUOS{
    friend class memory;
	public:

        class Process{
            public:
                Process(char * pN, State8080 rS, int pp);

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
                void increasePC(){
                    uint16_t inst = 7;
                    this->rState.pc +=inst;
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

            private:
                char processName[255];
                State8080 rState;         // Saved register set of 8080 chip
                uint32_t baseReg;
                uint32_t limitReg;
                int pid;                    // process id
                int ppid;                   // parent pid
                int wpid;                   // waiting for pid
                uint64_t SC;                // Starting time of the process(the cycle number of the CPU)
                int UC;                     // How many cycles the process has used so far
                ProcessStates pState;       // The state of the process (Ready, Blocked, Running)
        };

    uint64_t handleCall(CPU8080 & cpu, unsigned int CRPIndex);
    void printPhysicalMemoryToFile(const CPU8080 & cpu);
    void printMemoryToFile(unsigned int CRPIndex, const CPU8080 & cpu);
    void printProcessTable();
    void createProcess(CPU8080 & cpu, char *pN, int ppid);
    void finishProcess(int pTableIndex);
    int contentSwitch(CPU8080 & cpu, unsigned int & CRPIndex, int & blockedCycles, int & usedCycles, char *blockedPName, char* nextPName);

    int fork(CPU8080 & cpu, unsigned int CRPIndex);
    int exec(CPU8080 & cpu, unsigned int CRPIndex);
    int waitpid(CPU8080 & cpu, unsigned int CRPIndex);

    public:
        vector<Process*> pTable;    // Process Table
};

#endif