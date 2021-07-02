#include "Instructions_realizations.h"

const int MAX_REGISTERS_NUMBER = 7;

const int REG_REGADDR 		   = 0;
const int REG_REGADDR_8OFFSET  = 1;
const int REG_REGADDR_32OFFSET = 2;
const int REG_REG 			   = 3;

const char REX = 4;

const int REX_W = 0b1000;
const int REX_R = 0b0100;
const int REX_X = 0b0010;
const int REX_B = 0b0001;

const unsigned char UNARY_EXTEND = 0b0011;
const unsigned char SHL_EXTEND = 0x4;

const char SHIFT_EXTENSIONS[]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x04, 0x05, 0};
const char ONE_ARG_EXTENTION[] = {0, 0, 0, 0, 0x04, 0, 0, 0, 0, 0, 0, 0, 0x06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

extern const size_t MAX_MESSAGE_LENGTH = 10;


const unsigned char OPCODE[] = 
{
	0x89,//mov
	0x8b,//mov_from_memory
	0xc7,//mov_imm
	0x68,//push
	0x50,//push_reg
	0xff,//push_from_mem
	0x8f,//pop
	0x58,//pop_reg
	0x8f,//pop__to_mem
	0x01,//add
	0x29,//sub
	0xf7,//mul
	0xf7,//div
	0xe9,//jmp, rel32
	0x0f,//jae, rel32
	0x0f,//ja
	0x0f,//jb
	0x0f,//jbe
	0x0f,//je
	0xe8,//call
	0x39,//cmp two registers
	0xc3,//ret
	0x90,//nop
	0xc1,//shl
	0xc1,//shr
	0xcc //int 3
};

const unsigned char JUMPS_EXTENDED_OPCODES[] =
{
	0x83,
	0x87,
	0x82,
	0x86,
	0x84
};

const unsigned char STACK_COMMANDS_EXTENDED_OPCODES[] =
{
	0x00,
	0x00,
	0x06,
	0x00,
	0x00,
	0x00
};

Code_state_signs making_stack_register_command(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_REG || command_code == CODE_POP_REG);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (register_number > MAX_REGISTERS_NUMBER)
	{
		unsigned char WRXB = 0;
		WRXB |= REX_B;

		message[0] = (REX << 4) | WRXB; 
		report = copy_phrase(state, message, 1);
		MAKE_REPORT
	}

	message[0] = OPCODE[command_code] + register_number % (MAX_REGISTERS_NUMBER + 1);
	report = copy_phrase(state, message, 1);
	if (report != CODE_OK)
		return report;

	return CODE_OK;
}
					
Code_state_signs making_additional_stack_register_command(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_REG || command_code == CODE_POP_REG);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = REX_W;

	if (command_code == CODE_POP_REG)
	{
		WRXB = REX_W;
		
		report = making_command_mov_imm(state, RAX, R8, sizeof(long long));
		MAKE_REPORT
		report = making_simple_arithmetics(state, CODE_SUB, R8, RBX);
		MAKE_REPORT
	}

	if (register_number > MAX_REGISTERS_NUMBER)
	{
		WRXB |= REX_R;
	}

	message[0] = (REX << 4) | WRXB; 
	if (command_code == CODE_POP_REG)
	{
		message[1] = OPCODE[CODE_MOV_FROM_MEM];
	}
	else
	{
		message[1] = OPCODE[CODE_MOV];
	}
	message[2] = (REG_REGADDR << 6) |
				 (register_number % (MAX_REGISTERS_NUMBER + 1) << 3) |
				 4;
	message[3] = 0x1e;
	report = copy_phrase(state, message, 4);
	MAKE_REPORT

	
	if (command_code != CODE_POP_REG)
	{
		report = making_command_mov_imm(state, RAX, R8, sizeof(long long));
		MAKE_REPORT
		report = making_simple_arithmetics(state, CODE_ADD, R8, RBX);
		MAKE_REPORT
	}

	return CODE_OK;
}

Code_state_signs making_stack_imm_command(Code_state *state, int command_code, int value)
{
	assert(command_code == CODE_PUSH_IMM || command_code == CODE_POP_IMM);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (command_code == CODE_PUSH_IMM)
	{
		message[0] = OPCODE[command_code];
		report = copy_phrase(state, message, 1);
		MAKE_REPORT
		report = put_number(state, value);
		MAKE_REPORT
	}
	else
	{
		unsigned char WRXB = 0;
		WRXB |= REX_B;

		message[0] = (REX << 4) | WRXB; 
		message[1] = OPCODE[CODE_POP_REG] + R11 % (MAX_REGISTERS_NUMBER + 1);

		report = copy_phrase(state, message, 2);
		MAKE_REPORT
	}

	return CODE_OK;
}

Code_state_signs making_additional_stack_imm_command(Code_state *state, int command_code, int value)
{
	assert(command_code == CODE_PUSH_IMM || command_code == CODE_POP_IMM);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (command_code == CODE_PUSH_IMM)
	{
		report = making_command_mov_imm(state, RAX, RAX, value);
		MAKE_REPORT
		report = making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
		MAKE_REPORT
	}
	else
	{
		report = making_additional_stack_register_command(state, CODE_POP_REG, RAX);
		MAKE_REPORT
	}

	return CODE_OK;
}

Code_state_signs making_stack_register_address(Code_state *state, int command_code, int mod, int value)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = OPCODE[command_code];
	message[1] = (mod << 6) |
				 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
				 RDI;
	report = copy_phrase(state, message, 2);
	MAKE_REPORT

	if(mod == REG_REGADDR_32OFFSET)
	{
		report = put_number(state, value);
		MAKE_REPORT
	}

	return CODE_OK;
}

Code_state_signs making_additional_stack_register_address(Code_state *state, int command_code, int mod, int value)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	report = making_additional_stack_register_command(state, command_code, RDI);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_stack_register_address_in_register(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);
	assert(register_number < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	making_simple_arithmetics(state, CODE_ADD, RDI, register_number);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	if (register_number > MAX_REGISTERS_NUMBER)
	{
		unsigned char WRXB = REX_B;

		message[0] = (REX << 4) | WRXB;
		report = copy_phrase(state, message, 1);
		MAKE_REPORT
	}
	
	message[0] = OPCODE[command_code];
	message[1] = (REG_REGADDR << 6) |
				 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
				 (register_number % (MAX_REGISTERS_NUMBER + 1));

	report = copy_phrase(state, message, 2);
	MAKE_REPORT

	report = making_simple_arithmetics(state, CODE_SUB, RDI, register_number);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_left_shift(Code_state *state, int register_number, char shift)
{
	assert(register_number < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	//48 c1 e0 03 shl rax, 3
	unsigned char WRXB = REX_W;
	if (register_number > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = (REX << 4) | WRXB;
	message[1] = OPCODE[CODE_SHL];
	message[2] = (UNARY_EXTEND << 6) | 
				 (SHIFT_EXTENSIONS[CODE_SHL] << 3) | 
				 (register_number % (MAX_REGISTERS_NUMBER + 1));
	message[3] = shift;
	report = copy_phrase(state, message, 4);
	MAKE_REPORT

	return CODE_OK;
}

int making_valid_address_offset(Code_state *state, int register_number)
{
	assert(register_number < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	report = making_command_mov_from_reg(state, register_number, R9);
	report = making_left_shift(state, R9, 3);

	return R9;
}

Code_state_signs making_additional_stack_register_address_in_register(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);
	assert(register_number < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS
	
	int register_offset = making_valid_address_offset(state, register_number);

	report = making_simple_arithmetics(state, CODE_ADD, register_offset, RDI);
	MAKE_REPORT
	if (command_code == CODE_PUSH_MEM)
	{
		report = making_command_mov_from_mem(state, CODE_MOV_FROM_MEM, REG_REGADDR, RDI, RAX, 0);
		MAKE_REPORT
	}

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (command_code == CODE_PUSH_MEM)
	{
		report = making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
		MAKE_REPORT
	}
	else
	{
		report = making_additional_stack_register_command(state, CODE_POP_REG, RAX);
		MAKE_REPORT
	}
	if (command_code == CODE_POP_MEM)
	{
		report = making_command_mov_from_mem(state, CODE_MOV, REG_REGADDR, RDI, RAX, 0);
		MAKE_REPORT
	}

	report = making_simple_arithmetics(state, CODE_SUB, register_offset, RDI);
	MAKE_REPORT

	return CODE_OK;
}

//mov register_to, register_from
Code_state_signs making_command_mov_from_reg(Code_state *state, int register_number_from, int register_number_to)
{
	assert(register_number_from < (MAX_REGISTERS_NUMBER + 1) * 2 && register_number_to < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	if (register_number_from > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV]; 
	message[2] = (REG_REG << 6) | 
				 (register_number_from % (MAX_REGISTERS_NUMBER + 1) << 3) | 
				 (register_number_to % (MAX_REGISTERS_NUMBER + 1));

	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_command_mov_imm(Code_state *state, int register_number_from, int register_number_to, int immediate)
{
	assert(register_number_from < (MAX_REGISTERS_NUMBER + 1) * 2 && register_number_to < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;

	if (register_number_from > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV_IMM]; 
	message[2] = (REG_REG << 6) | 
			 	 (register_number_from % (MAX_REGISTERS_NUMBER + 1) << 3) | 
			     (register_number_to % (MAX_REGISTERS_NUMBER + 1));
	
	report = copy_phrase(state, message, 3);
	MAKE_REPORT
	
	report = put_number(state, immediate);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_command_mov_from_mem(Code_state *state, int operation_code, int mod, int register_number_from, int register_number_to, int immediate)
{
	assert(register_number_from < (MAX_REGISTERS_NUMBER + 1) * 2 && register_number_to < (MAX_REGISTERS_NUMBER + 1) * 2);
	assert(mod == REG_REGADDR || mod == REG_REGADDR_32OFFSET);
	assert(operation_code == CODE_MOV_FROM_MEM || operation_code == CODE_MOV);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	if (register_number_from > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (mod << 6) | 
			 	 (register_number_to % (MAX_REGISTERS_NUMBER + 1) << 3) | 
			     (register_number_from % (MAX_REGISTERS_NUMBER + 1));
	
	report = copy_phrase(state, message, 3);
	MAKE_REPORT
	
	if (mod == REG_REGADDR_32OFFSET)
	{
		report = put_number(state, immediate);
		MAKE_REPORT
	}

	return CODE_OK;
}

Code_state_signs prepare_registers_for_binary_instructions(Code_state *state)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	Code_state_signs report = making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_additional_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs push_back_return_register(Code_state *state)
{
	Code_state_signs report = making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
	MAKE_REPORT

	return CODE_OK;
}

const char Mods[] = 
{
	0b0000,
	0b0001,
	0b0010,
	0b0011
};

Code_state_signs mode_reg_rm_instrufctions(Code_state *state, int mod, int reg, int rm)
{
	// rm is last digit
	// d=0: MOD R/M <- REG, REG is the source

	// d=1: REG <- MOD R/M, REG is the destination

	return CODE_OK;
}

// add second_register, first_register
Code_state_signs making_simple_arithmetics(Code_state *state, int operation_code, int first_register, int second_register)
{
	assert(first_register < (MAX_REGISTERS_NUMBER + 1) * 2 && second_register < (MAX_REGISTERS_NUMBER + 1) * 2);
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	
	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	if (second_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	message[0] = REX << 4 | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (REG_REG << 6) | 
				 (first_register % (MAX_REGISTERS_NUMBER + 1) << 3) | 
				 (second_register % (MAX_REGISTERS_NUMBER + 1));
	
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_simple_arithmetics_with_stack(Code_state *state, int operation_code)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	report = making_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code];
	message[2] = (REG_REG << 6) | 
				 (RCX << 3) | 
				 (RAX);
	copy_phrase(state, message, 3);

	report = making_stack_register_command(state, CODE_PUSH_REG, RAX);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_simple_arithmetics_with_additional_stack(Code_state *state, int operation_code)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	report = making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_additional_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code];
	message[2] = (REG_REG << 6) | 
				 (RCX << 3) | 
				 (RAX);
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	report = making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_complex_arithmetics(Code_state *state, int mode, int register_number)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);
	assert(register_number < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (register_number % (MAX_REGISTERS_NUMBER + 1));
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_complex_arithmetics_with_stack(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	report = making_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (RCX);
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	report = making_stack_register_command(state, CODE_PUSH_REG, RAX);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_complex_arithmetics_with_additional_stack(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	report = making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_additional_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (RCX);
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	report = making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_comparing(Code_state *state, int first_register, int second_register)
{
	assert(first_register < (MAX_REGISTERS_NUMBER + 1) * 2 && second_register < (MAX_REGISTERS_NUMBER + 1) * 2);
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	if (second_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (3 << 6) | 
				 (first_register % (MAX_REGISTERS_NUMBER + 1) << 3) | 
				 (second_register % (MAX_REGISTERS_NUMBER + 1));
	
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_comparing_with_stack(Code_state *state)
{
	CHECKING_POINTERS

	report = making_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (0x3 << 6) | 
				 (RAX << 3) | 
				 (RCX);
	
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_comparing_with_additional_stack(Code_state *state)
{
	CHECKING_POINTERS

	report = making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	MAKE_REPORT
	report = making_additional_stack_register_command(state, CODE_POP_REG, RCX);
	MAKE_REPORT

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (0x3 << 6) | 
				 (RAX << 3) | 
				 (RCX);
	
	report = copy_phrase(state, message, 3);
	MAKE_REPORT

	return CODE_OK;
}


Code_state_signs making_command_jump(Code_state *state, int jump_length)
{
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	message[0] = OPCODE[CODE_JMP];
	report = copy_phrase(state, message, 1);
	if (report != CODE_OK)
		return report;

	report = put_number(state, jump_length);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_conditional_jump(Code_state *state, int jump_number, int address)
{
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = OPCODE[jump_number];
	message[1] = JUMPS_EXTENDED_OPCODES[jump_number - CODE_JAE];
	report = copy_phrase(state, message, 2);
	MAKE_REPORT

	report = put_number(state, address);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_call(Code_state *state, int address)
{
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	
	message[0] = OPCODE[CODE_CALL];
	report = copy_phrase(state, message, 1);
	MAKE_REPORT

	report = put_number(state, address);
	MAKE_REPORT

	return CODE_OK;
}

Code_state_signs making_ret(Code_state *state)
{
	CHECKING_POINTERS

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = OPCODE[CODE_RET];
	report = copy_phrase(state, message, 1);
	MAKE_REPORT

	return CODE_OK;
}