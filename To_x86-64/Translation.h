#ifndef TRANSLATION_STRUCTURES_FILE
#define TRANSLATION_STRUCTURES_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern const unsigned char REGISTERS_NUMBERS[];
extern const unsigned char AASM_REGISTERS_NUMBERS[];

extern const int translation_table[];

extern const unsigned char MY_PRINTF_MACHINE_CODE[];
extern const unsigned char MY_SCANF_MACHINE_CODE[];
extern const unsigned char MY_HLT_MACHINE_CODE[];

extern const int PRINTF_SIZE;
extern const int SCANF_SIZE;
extern const int HLT_SIZE;

extern const int PRINTF_ADDRESS;
extern const int SCANF_ADDRESS;
extern const int HLT_ADDRESS;
extern const int START_JUMP;

typedef enum commands_names
{
    COMMAND_HLT,
    COMMAND_PUSH,
    COMMAND_ADD,
    COMMAND_SUB,
    COMMAND_MUL,
    COMMAND_OUT,
    COMMAND_SIN,
    COMMAND_COS,
    COMMAND_POP,
    COMMAND_SQRT,
    COMMAND_IN,
    COMMAND_DUMP,
    COMMAND_DIV,
    COMMAND_JMP,
    COMMAND_JAE,
    COMMAND_JA,
    COMMAND_JB,
    COMMAND_JBE,
    COMMAND_JE,
    COMMAND_JNE,
    COMMAND_JM,
    COMMAND_CALL,
    COMMAND_RET
} commands_names;

extern const int STACK_PUSH_OFFSETS[];
extern const int STACK_POP_OFFSETS[];

// extern const size_t stack_registers;
extern const size_t MAX_STRING_LENGTH;

#include "Executable_elf.h"

#include "../File_work/ReadingFromFile.h"
#include "../Consts_and_structures/Consts.h"

#include "Instructions_realizations.h"
#include "Translation_structures.h"



Code_state_signs copy_phrase(Code_state *state, const unsigned char *phrase, const size_t phrase_length);
Code_state_signs put_number(Code_state *state, const int value);

Mod_reg *get_type(Code_state *state, int mode);
Code_state_signs translation_process_push(Code_state *state);
void translation_process_pop(Code_state *state);

void translation_process_simple_arithmetics(Code_state *state, int mode);
void translation_process_complex_arithmetics(Code_state *state, int mode);

void translation_process_print(Code_state *state);
void translation_process_scan(Code_state *state);
void translation_process_hlt(Code_state *state);

bool is_conditional_jump(int instruction_index);
int extract_long_value(Code_state *state);
void translation_process_jump(Code_state *state, int jump_number);
void translation_process_call(Code_state *state);
void translation_process_ret(Code_state *state);

void translation_text_header(Code_state *state);
void translation_counting_addresses(Code_state *state);
void translation_parser_code(Code_state *state);

void translation_into_x86_64(const char *file_name, const char *output_file_name);


#endif
