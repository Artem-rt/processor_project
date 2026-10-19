// decoder comands numbers------

// include guards---------------
#ifndef COMMANDS_H
#define COMMANDS_H
// -----------------------------

// enum -- commands codes-------
enum PROG_CODE
{
    NON = 0,
    PUSH = 1,
    PUSH_ARG = 2,
    POP = 3,
    POP_ARG = 4,
    ADD = 5,
    SUB= 6,
    MULL = 7,
    DIV = 8,
    OUT = 9,
    DUMP = 10,
    JMP = 11,
    JA = 12,
    JAE = 13,
    JB = 14,
    JBE = 15,
    JE = 16,
    JNE = 17,
    IN = 18,
    CALL = 20,
    RET = 21,
    HLT = 0xF4,
} ;
// ------------------------------

#endif
// THE END-----------------------
