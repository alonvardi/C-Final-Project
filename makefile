all: first.o second.o assembler.o assist.o symbolsTable.o memoryMap.o
	gcc -g -Wall -ansi symbolsTable.o memoryMap.o  assembler.o first.o second.o assist.o -o all

assembler.o : assembler.c  set.h
	gcc -g -ansi -pedantic -Wall -c assembler.c -o assembler.o

first.o: firstLoop.c set.h
	gcc -g -ansi -pedantic -Wall -c firstLoop.c -o first.o

assist.o: assistFunctions.c set.h
	gcc -g -ansi -pedantic -Wall -c assistFunctions.c -o assist.o
	
second.o: secondLoop.c set.h 
	gcc -g -ansi -pedantic -Wall -c secondLoop.c -o second.o
	
symbolsTable.o: symbolsTable.c set.h
	gcc -g -ansi -pedantic -Wall -c symbolsTable.c -o symbolsTable.o
	
memoryMap.o: memoryMap.c set.h
	gcc -g -ansi -pedantic -Wall -c memoryMap.c -o memoryMap.o
	


#cleanup
#clean:
#	rm *.o *~


