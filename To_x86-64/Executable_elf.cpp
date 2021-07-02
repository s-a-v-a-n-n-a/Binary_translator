#include "Executable_elf.h"

const long long USERS_ADDRESS_START   = 0x0000000000401000;
const long long COUNTING_MEMORY_START = 0x0000000000402000;
const long long DATA_ADDRESS_START    = 0x0000000000402000;

const long long ENTRY_ADDRESS = 0x0000000000001000;

size_t elf_generate_elf_header(unsigned char *address_to_load, const size_t available_size)
{
	Elf_header thus = {};

	if (available_size < sizeof(Elf_header))
		return 0;
	memcpy(address_to_load, (unsigned char*)&thus, sizeof(Elf_header) * sizeof(unsigned char));

	return sizeof(Elf_header);
}

size_t elf_generate_program_header(unsigned char *address_to_load, const size_t available_size, const size_t program_size)
{
	Program_header thus = {};
	thus.Prog_FLAGS = PF_X | PF_R | PF_W;
	thus.Prog_OFFSET = ENTRY_ADDRESS;
	thus.Prog_VADDR = USERS_ADDRESS_START;
	thus.Prog_FILESZ = program_size;
	thus.Prog_MEMSZ = program_size;

	//unsigned char *program_header = (unsigned char*)calloc(sizeof(Program_header), sizeof(unsigned char));
	//program_header[sizeof(Program_header)] = {};
	if (available_size < sizeof(Program_header))
		return 0;
	memcpy(address_to_load, (unsigned char*)&thus, sizeof(Program_header) * sizeof(unsigned char));

	return sizeof(Program_header);
}

size_t elf_generate_program_header_for_data(unsigned char *address_to_load, const size_t available_size, const size_t data_size)
{
	Program_header thus = {};
	thus.Prog_FLAGS = PF_W | PF_R;
	thus.Prog_OFFSET = 0;
	thus.Prog_VADDR = DATA_ADDRESS_START;//размер обоих буферов (может, сделать 2?)
	//thus.Prog_PADDR = 0;
	thus.Prog_FILESZ = 0;
	thus.Prog_MEMSZ = data_size;

	//unsigned char *program_header = (unsigned char*)calloc(sizeof(Program_header), sizeof(unsigned char));
	//program_header[sizeof(Program_header)] = {};
	if (available_size < sizeof(Program_header))
		return 0;
	memcpy(address_to_load, (unsigned char*)&thus, sizeof(Program_header) * sizeof(unsigned char));

	return sizeof(Program_header);
}