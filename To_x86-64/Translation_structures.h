#ifndef STRUCTURES
#define STRUCTURES

#include <cstdio>
#include <cstdlib>
#include <cstring>

enum Simple_list_state_signs
{
	SLIST_OK,
	SLIST_INVALID_POINTER,
	SLIST_MEMORY_ERROR
};

enum Code_state_signs
{
	CODE_OK,
	CODE_INVALID_POINTER,
	CODE_MEMORY_ERROR,
	CODE_FILE_ERROR
};

typedef struct Mod_reg     Mod_reg;
typedef struct Simple_list Simple_list;
typedef struct Code_state  Code_state;

extern const size_t INITIAL_LIST_SIZE;

extern const size_t FILE_LENGTH;
extern const size_t TMP_DATA_SIZE;

#include "Executable_elf.h"
#include "Translation.h"

#include "../File_work/ReadingFromFile.h"
#include "../Consts_and_structures/Consts.h"

typedef struct Mod_reg
{
	char type;
	char mod;
	char reg;
	bool address_existing;
	size_t address;
} Mod_reg;

typedef struct Simple_list
{
	int *array;

	size_t array_length;
	size_t array_capacity;
} Simple_list;

typedef struct Code_state
{
	unsigned char *x86_code;
	char *aasm_code;

	// int stop_place;

	size_t stack_counter;
	// size_t stack_registers_counter;

	size_t aasm_code_length;
	size_t rip;

	size_t code_length;
	size_t code_capacity;

	//int passage_number;

	Simple_list *addresses_storage;
	size_t addresses_counter;

	Simple_list *label_addresses;
	Simple_list *jump_addresses;
} Code_state;

Mod_reg *reg_state_mode_new();
void reg_state_mode_delete(Mod_reg *thus);

Simple_list 		   *simple_list_new(size_t array_length);
Simple_list_state_signs simple_list_construct(Simple_list *thus, size_t array_length);
Simple_list_state_signs simple_list_delete(Simple_list *thus);
Simple_list_state_signs simple_list_destruct(Simple_list *thus);
Simple_list_state_signs simple_list_insert(Simple_list *thus, int value, int index);
Simple_list_state_signs simple_list_resize(Simple_list *thus);
Simple_list_state_signs simple_list_check_pointers(Simple_list *thus);

Code_state *translation_new(size_t length, const char *file_name);
Code_state_signs translation_construct(Code_state *state, size_t length, const char *file_name);
Code_state_signs translation_delete(Code_state *state, const char *file_name);
Code_state_signs translation_destruct(Code_state *state);
Code_state_signs translation_resize(Code_state *state);;
Code_state_signs translation_check_pointers(Code_state *state);

#endif
