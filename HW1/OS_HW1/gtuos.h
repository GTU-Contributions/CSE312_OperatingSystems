// Mehmed Mustafa 131044084
// 13/03/2017
#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"


class GTUOS{
	public:
		uint64_t handleCall(const CPU8080 & cpu);
		void printMemoryToFile(const char* filename, const CPU8080 & cpu);
};

#endif
