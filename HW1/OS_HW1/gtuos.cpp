// Mehmed Mustafa 131044084
// 13/03/2017
#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;

uint64_t GTUOS::handleCall(const CPU8080 & cpu){

    uint64_t cycles;
	uint16_t BCaddress = (cpu.state->b << 8) | cpu.state->c;
    int value = -9999;
    char ch = 'm';
    int chCounter = 0;

	switch(cpu.state->a)
	{
		case 1:
			cout << "System Call: PRINT_B" << endl;
			cout << "Value of B " << (int)cpu.state->b << endl << endl;
            cycles = 10;
			break;
		case 2:
			cout << "System Call: PRINT_MEM" << endl;
			cout << "Value of MEM[BC] " << (int)cpu.memory[BCaddress] << endl << endl;
			cycles = 10;
			break;
		case 3:
			cout << "System Call: READ_B" << endl;
			cin >> value;
            cpu.state->b = (uint8_t)value;
            cout << "Readed " << value << " into B" << endl << endl;
            cycles = 10;
			break;
		case 4:
			cout << "System Call: READ_MEM" << endl;
			cin >> value;
            cpu.memory[BCaddress] = (uint8_t)value;
            cout << "Readed " << value << " into MEM[BC]" << endl << endl;
            cycles = 10;
			break;
		case 5:
			cout << "System Call: PRINT_STR" << endl;
			cout << "String: ";

            for(int i=0; ch != '\0'; ++i){
                ch = (char)cpu.memory[BCaddress+i];
                cout << ch;
            }
            cout << endl;
            cycles = 100;
			break;
		case 6:
			cout << "System Call: READ_STR" << endl;
            do{
				ch = getchar();
				cpu.memory[BCaddress+chCounter] = ch;
				++chCounter;
			}while(ch != '\n');
            cpu.memory[BCaddress+chCounter] = '\0';
            cycles = 100;
			break;
        default:
            cout << "Error: System call cannot be handled! Value of Register A[";
            cout << (int)cpu.state->a << "] is unexpected!" << endl;
	}

	return cycles;
}

void GTUOS::printMemoryToFile(const char* filename, const CPU8080 & cpu){
    FILE *filePtr = NULL;
    char memoryFileName[100];
    int newLineCounter = 0;

    // Get the filename part before .com
    memcpy(memoryFileName, filename, strlen(filename) - 4);
    memoryFileName[strlen(filename) -4] = '\0';
    // Add to the back of it .mem
    strcat(memoryFileName, ".mem");

	filePtr = fopen(memoryFileName, "w");
	if (filePtr == NULL)
	{
		printf("Error: Couldn't open %s\n", memoryFileName);
		exit(1);
	}

	fprintf(filePtr, "%5d. ", 0);
	for(int i=0; i<0x10000; ++i)
	{
		fprintf(filePtr, "%02x ", cpu.memory[i]);
		++newLineCounter;
		if(newLineCounter == 16){
			newLineCounter = 0;
			fprintf(filePtr, "\n%5d. ", i+1);
		}
	}
	
	fclose(filePtr);
}
