#ifndef EXECUTE_ELF_FILE
#define EXECUTE_ELF_FILE

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

typedef struct Elf_header
{
	unsigned int 		Elf_MAG 			= 0x464C457F; //magic numbers
	unsigned char 		Elf_CLASS 			= ELFCLASS64; //64-bit
	unsigned char 		Elf_DATA 			= ELFDATA2LSB; //little endian
	unsigned char 		Elf_VERSION 		= 0x01; //original and current version of ELF
	unsigned char 		Elf_OSABI 			= ELFOSABI_NONE; //Unix System V
	unsigned long long 	Elf_ABIVERSION_PAD 	= 0x0000000000000000; //unused
	unsigned short int 	Elf_TYPE 			= ET_EXEC; //executable
	unsigned short int 	Elf_MACHINE 		= EM_X86_64; //Amd x86-64
	unsigned int 		Elf_version 		= EV_CURRENT; //original version of ELF
	unsigned long long 	Elf_ENTRY 			= 0x0000000000401000; //Entry address
	unsigned long long 	Elf_PHOFF 			= 0x0000000000000040; //start of the program header table in 64-bit system
	unsigned long long 	Elf_SHOFF 			= 0x0000000000000000; //No need for us???
	unsigned int 		Elf_FLAGS 			= 0x00000000; //depends on architechture target
	unsigned short int 	Elf_EHSIZE 			= 0x0040; //size of elf header in 64-bit system
	unsigned short int 	Elf_PHENTSIZE 		= 0x0038; //program header size
	unsigned short int 	Elf_PHNUM 			= 0x0002; //number of entries in program
	unsigned short int 	Elf_SHENTSIZE 		= 0x0040; //size of the section header table
	unsigned short int 	Elf_SHNUM 			= 0x0000; //number of entries in the section header table
	unsigned short int 	Elf_SHSTRNDX 		= 0x0000; //index of the section header table entry that contains the section names
}Elf_header;

typedef struct Program_header
{
	unsigned int 		Prog_TYPE 	= PT_LOAD; //header table entry
	unsigned int 		Prog_FLAGS 	= 0x00000000;
	unsigned long long 	Prog_OFFSET = 0x0000000000000000; //Physical offset from beginning of elf
	unsigned long long 	Prog_VADDR 	= 0x0000000000400000; //Virtual address of the segment in memory
	unsigned long long 	Prog_PADDR 	= 0x0000000000400000; //Physical address
	unsigned long long 	Prog_FILESZ = 0x0000000000000080;
	unsigned long long 	Prog_MEMSZ 	= 0x0000000000000080;
	unsigned long long 	Prog_ALIGN 	= 0x0000000000000001;
}Program_header;

extern const long long USERS_ADDRESS_START;
extern const long long DATA_ADDRESS_START;
extern const long long ENTRY_ADDRESS;

size_t elf_generate_elf_header(unsigned char *address_to_load, const size_t available_size);
size_t elf_generate_program_header(unsigned char *address_to_load, const size_t available_size, const size_t program_size);
size_t elf_generate_program_header_for_data(unsigned char *address_to_load, const size_t available_size, const size_t data_size);

#endif