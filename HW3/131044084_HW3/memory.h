// Mehmed Mustafa 131044084
// 15/04/2017
#ifndef H_MEMORY
#define H_MEMORY

#include "memoryBase.h"
#include <vector>
#include <iomanip>
#include "harddisk.h"

using namespace std;

class memory: public MemoryBase {
    friend class GTUOS;
    public:
        class Page{
            public:
                Page(int cVPN, int cPPN, bool present, bool modified){
                    this->VPN = cVPN;
                    this->PPN = cPPN;
                    this->present = present;
                    this->modified = modified;
                }
                void setPresent(bool value){this->present = value;}
                bool getPresent(){return present;};
                void setModified(bool value){this->modified = value;}
                bool getModified(){return modified;}

                void setVPN(int num){this->VPN = num;}
                int getVPN(){return VPN;}
                void setPPN(int num){this->PPN = num;}
                int getPPN(){return PPN;}

            private:
                bool present;  // States if the page is present in the memory
                bool modified; // States if the page is modified or not
                int VPN;  // Virtual Page Number - number between 0-63 for 64KB Virtual Memory
                int PPN;  // Physical Page Number - number between 0-7 for 8KB  Physical Memory
        };

        memory(harddisk * h){
            hard = h;

            mem = (uint8_t*) malloc(RAM_SIZE);
            for(unsigned i = 0; i<RAM_SIZE; ++i){
                mem[i] = 0;
            }
        }
		~memory(){ free(mem);}
		virtual uint8_t & at(uint32_t index);
		virtual uint8_t & physicalAt(uint32_t index);

        // Calculate in which page number the given address is in
        static int calculatePageNumber(int address){return address/PAGE_SIZE;}
        // Calculate Starting Address of a page number
        static int calculateSA(int pageNumber){return pageNumber*PAGE_SIZE;}
        // Calculate Ending Address of a page number
        static int calculateEA(int pageNumber){return (calculateSA(pageNumber) + PAGE_SIZE-1);}

        // Prints only pages which are presented in the memory
        void printPageTable();
        // Check's if the Page is in the Memory if so
        // returns the Physical Page Number, else returns NOT_IN_RAM(-1)
        int isPagePresent(int VPN);

        // Load 1KB page into Memory
        void load1KBpage(int VPN, int PPN);
        // Store 1KB page Into Hard Disk
        void store1KBpage(int VPN, int PPN);
        // Map Virtual to Physical Page FIFO Algorithm
        void mapVTP(int VPN);

        harddisk * hard;           // Hard Disk
    private:
        vector<Page*> pageTable;   // Page Table
		uint8_t * mem;             // Memory

};

#endif