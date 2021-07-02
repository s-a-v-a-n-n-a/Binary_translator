CFLAGS = -g -masm=intel -no-pie -march=native -O1

all: main.o translator.o file_reader.o elf.o consts.o instructions.o structures.o
	g++ main.o translator.o file_reader.o elf.o consts.o instructions.o structures.o -o translator

main.o : main.cpp
	g++ $(CFLAGS) -c -o main.o main.cpp

translator.o: To_x86-64/Translation.cpp
	g++ $(CFLAGS) -c -o translator.o To_x86-64/Translation.cpp

file_reader.o: File_work/ReadingFromFile.cpp
	g++ $(CFLAGS) -c -o file_reader.o File_work/ReadingFromFile.cpp

consts.o: ./Consts_and_structures/Consts.cpp
	g++ $(CFLAGS) -c -o consts.o ./Consts_and_structures/Consts.cpp

instructions.o: To_x86-64/Instructions_realizatons.cpp
	g++ $(CFLAGS) -c -o instructions.o To_x86-64/Instructions_realizatons.cpp

structures.o: To_x86-64/Translation_structures.cpp
	g++ $(CFLAGS) -c -o structures.o To_x86-64/Translation_structures.cpp

elf.o: To_x86-64/Executable_elf.cpp
	g++ $(CFLAGS) -c -o elf.o To_x86-64/Executable_elf.cpp

clean: 
	rm *.o

run: all
	./translator
