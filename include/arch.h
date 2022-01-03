#pragma once

enum OPCODES
{
    OP_NOP,
    OP_LDRIMM,
    OP_LDRMEM,
    OP_STR,
    OP_HLT,
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_CMP,
    OP_JZ,
    OP_JNZ,
    OP_JS,
    OP_JNS,
    OP_JMP,
    OP_PUSH,
    OP_POP,
    OP_CALL,
    OP_RET,
    OPCNT
};
