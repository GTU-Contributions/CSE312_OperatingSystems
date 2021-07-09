// Mehmed Mustafa 131044084
// 15/04/2017
#ifndef H_MEMBASE
#define H_MEMBASE

#if defined __UINT32_MAX__ or UINT32_MAX
  #include <inttypes.h>
#else
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned long uint32_t;
  typedef unsigned long long uint64_t;
#endif

#define HARD_DISK_SIZE 0x10000  // 64KB
#define RAM_SIZE       0x2000  // 8KB
#define PAGE_SIZE      0x400  // 1KB
#define NOT_IN_RAM     -1    // Used as default value for every virtual page

extern uint32_t CPBase_GLOBAL;  // Current Process Base Register
extern uint32_t CPLimit_GLOBAL; // Current Process Limit Register
extern uint32_t MM_CurrBase;    // To know the virtual place of the next created process
extern long long unsigned int totalCPUCycles; // Total Cycles from the CPU

extern uint64_t pagesReplaced;  // Holds the total number of the replaced pages
extern uint64_t pageFaults;     // Holds the total number of page faults
extern bool PAGE_FAULT;
extern bool PAGES_REPLACED;
extern int oldVPN, oldPPN, newVPN, newPPN;

class MemoryBase {
public:
        MemoryBase(){};
		virtual uint8_t & at(uint32_t) = 0;
		virtual uint8_t & physicalAt(uint32_t) = 0;

};

#endif


