#ifndef FILE_WITH_CONSTS
#define FILE_WITH_CONSTS

#define DEFINE_COMMANDS(name, number, arg, coding, discoding) \
        COM_##name,

typedef enum commands_for_processor
{
    #include "Commands.h"
} commands;

#undef DEFINE_COMMANDS

extern const char *LISTING_FILE;
extern const char *EXECUTABLE_FILE;
extern const char *ASSEMBLING_FILE_NAME;
extern const char *DISASSEMBLING_FILE;

extern const int START_NUMBER;
extern const int REGISTER_NUM;
extern const int RAM_MEMORY;

extern const int ONLY_VAL;
extern const int REG_RAX;
extern const int REG_RBX;
extern const int REG_RCX;
extern const int REG_RDX;
extern const int NO_REG_JUMP;
extern const int ADDRSS;
extern const int ADDRSS_RAX;
extern const int ADDRSS_RBX;
extern const int ADDRSS_RCX;
extern const int ADDRSS_RDX;
extern const int NOTHING;

extern const int MINUS;

#endif
