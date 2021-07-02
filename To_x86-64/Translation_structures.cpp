#include "Translation_structures.h"

const size_t INITIAL_LIST_SIZE = 10;

const size_t FILE_LENGTH   = 9000;
const size_t TMP_DATA_SIZE = 0x1000;

Mod_reg *reg_state_mode_new()
{
	Mod_reg *new_one = (Mod_reg*)calloc(1, sizeof(Mod_reg));
	if (!new_one)
		return NULL;

	return new_one;
}

void reg_state_mode_delete(Mod_reg *thus)
{
	if (thus)
		free(thus);
}

Simple_list *simple_list_new(size_t array_length)
{
	Simple_list *new_storage = (Simple_list*)calloc(1, sizeof(Simple_list));
	if (!new_storage)
		return NULL;
	
	Simple_list_state_signs report = simple_list_construct(new_storage, array_length);
	if (report != SLIST_OK)
	{
		free(new_storage);
		return NULL;
	}

	return new_storage;
}

Simple_list_state_signs simple_list_construct(Simple_list *thus, size_t array_length)
{
	if (!thus)
		return SLIST_INVALID_POINTER;

	thus->array = (int*)calloc(array_length, sizeof(int));
	if (!thus->array)
		return SLIST_MEMORY_ERROR;

	thus->array_length = 0;
	thus->array_capacity = array_length;

	return SLIST_OK;
}

Simple_list_state_signs simple_list_delete(Simple_list *thus)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	report = simple_list_destruct(thus);
	if (report != SLIST_OK)
		return report;
	free(thus);

	return SLIST_OK;
}

Simple_list_state_signs simple_list_destruct(Simple_list *thus)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	free(thus->array);

	return SLIST_OK;
}

Simple_list_state_signs simple_list_insert(Simple_list *thus, int value, int index)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	if (thus->array_length + 1 >= thus->array_capacity)
	{
		report = simple_list_resize(thus);
		if (report != SLIST_OK)
			return report;
	}

	while (index > thus->array_capacity)
	{
		report = simple_list_resize(thus);
		if (report != SLIST_OK)
			return report;
	}

	thus->array[index] = value;
	thus->array_length++;

	return SLIST_OK;
}

Simple_list_state_signs simple_list_resize(Simple_list *thus)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	size_t new_array_capacity = thus->array_capacity * 2;
	int *new_array = (int*)realloc(thus->array, new_array_capacity * sizeof(int));
	if (!new_array)
		return SLIST_INVALID_POINTER;

	if (new_array)
	{
		thus->array = new_array;
		thus->array_capacity = new_array_capacity;
	}	

	return SLIST_OK;
}

Simple_list_state_signs simple_list_check_pointers(Simple_list *thus)
{
	if (!thus || !thus->array)
		return SLIST_INVALID_POINTER;

	return SLIST_OK;
}



Code_state *translation_new(size_t length, const char *file_name)
{
	Code_state *new_state = (Code_state*)calloc(1, sizeof(Code_state));
	if (!new_state)
		return NULL;

	Code_state_signs report = translation_construct(new_state, length, file_name);
	if (report != CODE_OK)
	{
		free(new_state);
		return NULL;
	}
	return new_state;
}

Code_state_signs translation_construct(Code_state *state, size_t length, const char *file_name)
{
	if (!state)
		return CODE_INVALID_POINTER;

	size_t n_lines = 0;
	state->aasm_code = reading_file(file_name, &state->aasm_code_length, &n_lines, 0);
	if (!state->aasm_code)
		return CODE_MEMORY_ERROR;

	state->x86_code = (unsigned char*)calloc(length, sizeof(unsigned char));
	if (!state->x86_code)
		return CODE_MEMORY_ERROR;
	state->code_capacity = length;

	state->code_length   = 0;
	state->stack_counter = 0;

	state->rip = 0;

	state->addresses_storage = simple_list_new(INITIAL_LIST_SIZE);
	state->jump_addresses    = simple_list_new(INITIAL_LIST_SIZE);
	state->label_addresses   = simple_list_new(INITIAL_LIST_SIZE);

	if (!state->addresses_storage || !state->jump_addresses || !state->label_addresses)
		return CODE_MEMORY_ERROR;

	return CODE_OK;
}

Code_state_signs translation_delete(Code_state *state, const char *file_name)
{
	Code_state_signs report = translation_check_pointers(state);
	if (report != CODE_OK)
		return report;

	FILE *output = fopen(file_name, "wb");
	if (!output)
		return CODE_FILE_ERROR;
	
	unsigned char *final_byte_code = (unsigned char*)calloc(FILE_LENGTH, sizeof(unsigned char));
	if (!final_byte_code)
		return CODE_MEMORY_ERROR;

	size_t memory_loaded = elf_generate_elf_header(final_byte_code, FILE_LENGTH);
	if (memory_loaded == 0)
	{
		return CODE_MEMORY_ERROR;
	}
	else
	{
		size_t new_memory_loaded = elf_generate_program_header(final_byte_code + memory_loaded, FILE_LENGTH - memory_loaded, state->code_length);
		if (new_memory_loaded == 0)
		{
			return CODE_MEMORY_ERROR;
		}
		else
		{
			memory_loaded += new_memory_loaded;
			new_memory_loaded = elf_generate_program_header_for_data(final_byte_code + memory_loaded, FILE_LENGTH - memory_loaded, 0x4000);
			if (new_memory_loaded == 0)
			{
				return CODE_MEMORY_ERROR;
			}
			else
				memory_loaded += new_memory_loaded;
		}
	}
	memcpy(final_byte_code + ENTRY_ADDRESS, state->x86_code, state->code_length * sizeof(unsigned char));

	put_line(output, final_byte_code, FILE_LENGTH);
	
	free(final_byte_code);
	fclose(output);

	report = translation_destruct(state);
	if (report != CODE_OK)
		return report;

	free(state);

	return CODE_OK;
}

#define CORRELATE_ADDRESS_ARRAY_AND_CODE\
	if (address_report == SLIST_MEMORY_ERROR)\
		return CODE_MEMORY_ERROR;\
	else if (address_report == SLIST_INVALID_POINTER)\
		return CODE_INVALID_POINTER;\

Code_state_signs translation_destruct(Code_state *state)
{
	Code_state_signs report = translation_check_pointers(state);
	if (report != CODE_OK)
		return report;

	Simple_list_state_signs address_report = SLIST_OK;

	free(state->aasm_code);
	free(state->x86_code);
	
	address_report = simple_list_delete(state->addresses_storage);
	CORRELATE_ADDRESS_ARRAY_AND_CODE
	
	address_report = simple_list_delete(state->label_addresses);
	CORRELATE_ADDRESS_ARRAY_AND_CODE
	
	address_report = simple_list_delete(state->jump_addresses);
	CORRELATE_ADDRESS_ARRAY_AND_CODE

	return CODE_OK;
}

Code_state_signs translation_resize(Code_state *state)
{
	size_t new_capacity = state->code_capacity * 2;
	unsigned char *new_code = (unsigned char*)realloc(state->x86_code, new_capacity * sizeof(unsigned char));
	if (!new_code)
		return CODE_MEMORY_ERROR;

	if (new_code)
	{
		state->x86_code = new_code;
		state->code_capacity = new_capacity;
	}

	return CODE_OK;
}

Code_state_signs translation_check_pointers(Code_state *state)
{
	if (!state || !state->x86_code || !state->aasm_code)
		return CODE_INVALID_POINTER;

	if (simple_list_check_pointers(state->addresses_storage) != SLIST_OK ||
		simple_list_check_pointers(state->label_addresses) != SLIST_OK  ||
		simple_list_check_pointers(state->jump_addresses) != SLIST_OK)
		return CODE_INVALID_POINTER;

	return CODE_OK;
}