#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <arch.h>

#define REGCNT 8
#define MEMSIZE 65536

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
void op_cmp();
void op_jz();
void op_jnz();
void op_js();
void op_jns();
void op_jmp();

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
    op_xor,
    op_cmp,
    op_jz,
    op_jnz,
    op_js,
    op_jns,
    op_jmp
};

uint8_t regs[REGCNT] = { 0 };
uint8_t mem[MEMSIZE] = { 0 };
uint16_t pc = 0;
uint8_t flags = 0;

#define ZF (1 << 0)
#define SF (1 << 1)

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
    regs[r] = imm;
    printf("LDR $%d, %%r%d\n", imm, r);
}

void op_ldrmem()
{
    int src = getbyte();
    int dst = getbyte();
    regs[dst] = mem[(regs[7] << 8) | regs[src]];
    printf("LDR (%%r%d), %%r%d\n", src, dst);
}

void op_str()
{
    int src = getbyte();
    int dst = getbyte();
    mem[(regs[7] << 8) | regs[dst]] = regs[src];
    printf("STR %%r%d, (%%r%d)\n", src, dst);
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

    regs[dst] = regs[src];
    printf("MOV %%r%d, %%r%d\n", src, dst);
}

void op_add()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] += regs[src];
    printf("ADD %%r%d, %%r%d\n", src, dst);
}

void op_sub()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] -= regs[src];
    printf("SUB %%r%d, %%r%d\n", src, dst);
}

void op_mul()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] *= regs[src];
    printf("MUL %%r%d, %%r%d\n", src, dst);
}

void op_div()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] /= regs[src];
    printf("DIV %%r%d, %%r%d\n", src, dst);
}

void op_and()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] &= regs[src];
    printf("AND %%r%d, %%r%d\n", src, dst);
}

void op_or()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] |= regs[src];
    printf("OR %%r%d, %%r%d\n", src, dst);
}

void op_xor()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] ^= regs[src];
    printf("XOR %%r%d, %%r%d\n", src, dst);
}

void op_cmp()
{
    int src = getbyte(), dst = getbyte();
    int8_t res = regs[dst] - regs[src];

    if (res == 0) flags |= ZF;
    else if (res < 0) flags |= SF;
    
    printf("CMP %%r%d, %%r%d\n", src, dst);
}

void op_jz()
{
    uint8_t loc = getbyte();
    if (flags & ZF) pc = loc;

    printf("JZ $%d\n", loc);
}

void op_jnz()
{
    uint8_t loc = getbyte();
    if (!(flags & ZF)) pc = loc;

    printf("JNZ $%d\n", loc);
}

void op_js()
{
    uint8_t loc = getbyte();
    if (flags & SF) pc = loc;
    
    printf("JS $%d\n", loc);
}

void op_jns()
{
    uint8_t loc = getbyte();
    if (!(flags & SF)) pc = loc;

    printf("JNS $%d\n", loc);
}

void op_jmp()
{
    pc = (getbyte() << 8) | getbyte();
    printf("JMP $%d\n", pc);
}
