all :
	g++ -c -ansi -pedantic-errors 8080emu.cpp
	g++ -c -ansi -pedantic-errors gtuos.cpp
	g++ -c -ansi -pedantic-errors main.cpp
	
	g++ 8080emu.o gtuos.o main.o -o sim8080 -lm

	rm *.o
