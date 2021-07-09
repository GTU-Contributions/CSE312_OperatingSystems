#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

int main (int argc, char**argv)
{
	/*
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1); 
	}
	*/

    int DEBUG = 0;
	//int DEBUG = atoi(argv[2]);
    int totalCycles = 0;

	CPU8080 theCPU;
	GTUOS	theOS;

    theCPU.ReadFileIntoMemoryAt("Sort.com", 0x0000);
	//theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);

	do	
	{
		totalCycles += theCPU.Emulate8080p(DEBUG);
		if(theCPU.isSystemCall())
			totalCycles += theOS.handleCall(theCPU);

        if(DEBUG == 2)
            std::cin.get();

	}while (!theCPU.isHalted());

    std::cout << "Total Number of Cycles " << totalCycles << std::endl;

    theOS.printMemoryToFile("Sort.com", theCPU);
    //theOS.printMemoryToFile(argv[1], theCPU);

	return 0;
}
