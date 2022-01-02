#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define REGCNT 8
#define MEMSIZE 256

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
    OPCNT
};

void op_nop();
void op_ldrimm();
void op_ldrmem();
void op_str();
void op_hlt();
void op_mov();
void op_add();
void op_sub();
void op_mul();
void op_div();
void op_and();
void op_or();
void op_xor();

void (*opfuncs[OPCNT])() =
{
    op_nop,
    op_ldrimm,
    op_ldrmem,
    op_str,
    op_hlt,
    op_mov,
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_and,
    op_or,
    op_xor
};

uint8_t regs[REGCNT] = { 0 };
uint8_t mem[MEMSIZE] = { 0 };
uint8_t pc = 0;

int getbyte()
{
    return mem[pc++];
}

int doinst();

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: vm <romfile>\n");
        return -1;
    }

    FILE *rom = fopen(argv[1], "r");
    if (!rom)
    {
        char buf[64];
        snprintf(buf, 64, "vm: %s", argv[1]);
        perror(buf);
        return -1;
    }

    fseek(rom, 0, SEEK_END);
    size_t len = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    uint8_t *data = malloc(len);
    fread(data, len, 1, rom);
    fclose(rom);

    memcpy(mem, data, len);

    while (!doinst());
    return 0;
}

int doinst()
{
    int opcode = getbyte();
    if (opcode == -1) return -1;

    opfuncs[opcode]();
    return 0;
}

void op_nop()
{
    printf("NOP\n");
}

void op_ldrimm()
{
    uint8_t imm = (uint8_t)getbyte();
    int r = getbyte();
    printf("LDR $%d, %%r%d\n", imm, r);
    
    regs[r] = imm;
}

void op_ldrmem()
{
    int src = getbyte();
    int dst = getbyte();
    printf("LDR (%%r%d), %%r%d\n", src, dst);

    regs[dst] = mem[regs[src]];
}

void op_str()
{
    int src = getbyte();
    int dst = getbyte();
    printf("STR %%r%d, (%%r%d)\n", src, dst);

    mem[regs[dst]] = regs[src];
}

void op_hlt()
{
    printf("HLT\n");
    for (;;);
}

void op_mov()
{
    int src = getbyte();
    int dst = getbyte();
    printf("MOV(%%r%d, %%r%d)\n", src, dst);

    regs[dst] = regs[src];
}

void op_add()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] += regs[src];
}

void op_sub()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] -= regs[src];
}

void op_mul()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] *= regs[src];
}

void op_div()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] /= regs[src];
}

void op_and()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] &= regs[src];
}

void op_or()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] |= regs[src];
}

void op_xor()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] ^= regs[src];
}
