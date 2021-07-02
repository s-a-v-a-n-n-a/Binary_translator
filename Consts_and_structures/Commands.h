#define READ_MODE                                               \
    mode = *program_copy;                                       \
    program_copy++;                                             \
    rip++;

#define READ_VALUE(value, type);                                \
    for (int k = 0; k < type; k++)                              \
    {                                                           \
        ((char*)(&value))[k] = *program_copy++;                 \
    }                                                           \
    rip += type;


#define JUMP                                                    \
    program_copy = program_copy - (rip - (long long)jump) - 1;  \
    rip = (long long)jump - 1;                                  \


#define JUMP_STATEMENT(operator);                               \
    READ_VALUE(jump, sizeof(long long));                        \
                                                                \
    if (proc->stack->stack->length > 1)                         \
    {                                                           \
        stack_pop(&proc->stack, &val_last);                     \
        stack_pop(&proc->stack, &val_earl);                     \
                                                                \
        if (val_last operator val_earl)                         \
        {                                                       \
            JUMP                                                \
        }                                                       \
                                                                \
    }

DEFINE_COMMANDS ( HLT, 0, 0,
{
    stack_destruct(&proc->funcs);
    stack_destruct(&proc->stack);
    free(program);
    return;
},

{
    PRINT_DISASM("HLT\n");
})

DEFINE_COMMANDS ( PUSH, 1, 2,
{
    READ_MODE

    if ((int)mode && (int)mode <= REG_RDX)
    {
        stack_push(&proc->stack, proc->registers[(int)mode - 1]);
    }
    else if ((int)mode && (int)mode >= ADDRSS_RAX && (int)mode <= ADDRSS_RDX)
    {
        long long address = proc->registers[(int)mode - MINUS - 1];
        stack_push(&proc->stack, proc->ram[address]);
    }
    else if ((int)mode == ADDRSS)
    {
        READ_VALUE(val_last, sizeof(long long));

        stack_push(&proc->stack, proc->ram[(int)val_last]);
    }
    else
    {
        READ_VALUE(val_last, sizeof(double));

        stack_push(&proc->stack, val_last);
    }
},

{
    PRINT_DISASM("PUSH ");

    READ_MODE

    if ((int)mode > 0 && (int)mode < NO_REG_JUMP)
    {
        PRINT_REG
    }
    else if ((int)mode == ADDRSS && i == DISASSEMBLING)
    {
        READ_VALUE(val_last, sizeof(long long));

        if (i == DISASSEMBLING)                      //Плохо, убрать
            fprintf(dis, "[%lg]\n", val_last);
    }
    else
    {
        READ_VALUE(val_last, sizeof(double));

        if (i == DISASSEMBLING)
            fprintf(dis, "%lg\n", val_last);
    }
})

DEFINE_COMMANDS ( ADD, 2, 0,
{
    stack_pop(&proc->stack, &val_last);
    stack_pop(&proc->stack, &val_earl);

    stack_push(&proc->stack, val_earl + val_last);
},

{
    PRINT_DISASM("ADD\n");
})

DEFINE_COMMANDS ( SUB, 3, 0,
{
    stack_pop(&proc->stack, &val_last);
    stack_pop(&proc->stack, &val_earl);

    stack_push(&proc->stack, val_last - val_earl);
},

{
    PRINT_DISASM("SUB\n");
})

DEFINE_COMMANDS ( MUL, 4, 0,
{
    stack_pop(&proc->stack, &val_last);
    stack_pop(&proc->stack, &val_earl);

    stack_push(&proc->stack, val_last * val_earl);
},

{
    PRINT_DISASM("MUL\n");
})

DEFINE_COMMANDS ( OUT, 5, 0,
{
    stack_back(&proc->stack, &val_last);
    printf("out %lg\n", val_last);

    system("pause");
},

{
    PRINT_DISASM("OUT\n");
})

DEFINE_COMMANDS ( SIN, 6, 0,
{
    stack_pop(&proc->stack, &val_last);

    stack_push(&proc->stack, sin(val_last));
},

{
    PRINT_DISASM("SIN\n");
})

DEFINE_COMMANDS ( COS, 7, 0,
{
    stack_pop(&proc->stack, &val_last);

    stack_push(&proc->stack, cos(val_last));
},

{
    PRINT_DISASM("COS\n");
})

DEFINE_COMMANDS ( POP, 8, 1,
{
    READ_MODE

    if ((int)mode && (int)mode < NO_REG_JUMP)
    {
        stack_pop(&proc->stack, &val_last);
        proc->registers[(int)mode - 1] = val_last;
    }
    else if ((int)mode && (int)mode >= ADDRSS_RAX && (int)mode <= ADDRSS_RDX)
    {
        long long address = proc->registers[(int)mode - MINUS - 1];
        stack_pop(&proc->stack, &val_last);
        proc->ram[address] = val_last;
    }
    else if ((int)mode == ADDRSS)
    {
        READ_VALUE(val_last, sizeof(long long));

        stack_pop(&proc->stack, &val_earl);

        proc->ram[(int)val_last] = val_earl;
    }
    else
    {
        stack_pop(&proc->stack, &val_last);
    }
},

{
    READ_MODE

    PRINT_DISASM("POP ");

    if ((int)mode && (int)mode < NO_REG_JUMP)
    {
        PRINT_REG
    }
    else if ((int)mode == ADDRSS && i == DISASSEMBLING)       //Плохо
    {
        READ_VALUE(val_last, sizeof(long long));

        if (i == DISASSEMBLING)
            fprintf(dis, "[%lg]\n", val_last);
    }
    else if (i == DISASSEMBLING)
         fprintf(dis, "\n");
})

DEFINE_COMMANDS ( SQRT, 9, 0,
{
    stack_pop(&proc->stack, &val_last);

    stack_push(&proc->stack, sqrt(val_last));
},

{
    if (i == 2)
        fprintf(dis, "SQRT\n");
})

DEFINE_COMMANDS ( IN, 10, 0,
{
    {
        printf("Write your value: ");
        int prob = scanf("%lg", &val_last);

        if (prob != 1)
        {
            return;
        }

        stack_push(&proc->stack, val_last);
    }
},

{
    PRINT_DISASM("IN\n");
})

DEFINE_COMMANDS ( DUMP, 11, 0,
{
    stack_dump(proc->stack, STACK_OK, "PROCESSOR");
},

{
    PRINT_DISASM("DUMP\n");
})


DEFINE_COMMANDS ( DIV, 12, 0,
{
    stack_pop(&proc->stack, &val_last);
    stack_pop(&proc->stack, &val_earl);

    stack_push(&proc->stack, val_last / val_earl);
},

{
    PRINT_DISASM("DIV\n");
})

DEFINE_COMMANDS ( JMP, 13, 1,
{
    READ_VALUE(jump, sizeof(long long));

    JUMP
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JMP #%d\n");
})

DEFINE_COMMANDS ( JAE, 14, 1,
{
    JUMP_STATEMENT(>=);
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JAE #%d\n");
})

DEFINE_COMMANDS ( JA, 15, 1,
{
    JUMP_STATEMENT(>);
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JA #%d\n");
})

DEFINE_COMMANDS ( JB, 16, 1,
{
    JUMP_STATEMENT(<);
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JB #%d\n");
})

DEFINE_COMMANDS ( JBE, 17, 1,
{
   JUMP_STATEMENT(<=);
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JBE #%d\n");
})

DEFINE_COMMANDS ( JE, 18, 1,
{
    JUMP_STATEMENT(==);
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JE #%d\n");
})

DEFINE_COMMANDS ( JNE, 19, 1,
{
    JUMP_STATEMENT(!=);
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JNE #%d\n");
})

DEFINE_COMMANDS ( JM, 20, 1,
{
    {
        READ_VALUE(jump, sizeof(long long));

        struct tm *local;
        time_t timer = time(NULL);

        local = localtime(&timer);

        if (local->tm_wday == 1)
        {
            JUMP
        }
    }
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("JM #%d\n");
})

DEFINE_COMMANDS ( CALL, 21, 1,
{
    READ_VALUE(jump, sizeof(long long));

    stack_push(&proc->funcs, (stack_elem)rip);

    JUMP
},

{
    READ_VALUE(jump, sizeof(long long));

    COUNT_JUMPS

    PRINT_DISASM_JUMP("CALL #%d\n");
})

DEFINE_COMMANDS ( REV, 22, 0,
{
    double jmp = 0;
    stack_pop(&proc->funcs, &jmp);

    program_copy = program_copy - (rip - (long int)jmp);
    rip = (long int)jmp;
},

{
    PRINT_DISASM("REV\n");
})
