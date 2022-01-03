#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <arch.h>
#include <vm.h>

#define REGCNT 8
#define MEMSIZE 64000

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

//#define DEBUG

void info(const char *format, ...)
{
#ifdef DEBUG
    va_list list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
#else
    (void)format;
#endif
}

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

    gfx_init();
    while (!doinst())
    {
        gfx_update(mem);
    }
    gfx_fini();

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
    info("NOP\n");
}

void op_ldrimm()
{
    uint8_t imm = (uint8_t)getbyte();
    int r = getbyte();
    regs[r] = imm;
    info("LDR $%d, %%r%d\n", imm, r);
}

void op_ldrmem()
{
    int src = getbyte();
    int dst = getbyte();
    regs[dst] = mem[(regs[7] << 8) | regs[src]];
    info("LDR (%%r%d), %%r%d\n", src, dst);
}

void op_str()
{
    int src = getbyte();
    int dst = getbyte();

    if (((regs[7] << 8) | regs[dst]) < 256)
    {
        printf("%c", regs[src]);
        fflush(stdout);
    }
    else
        mem[(regs[7] << 8) | regs[dst]] = regs[src];
    
    info("STR %%r%d, (%%r%d)\n", src, dst);
}

void op_hlt()
{
    info("HLT\n");
    for (;;)
    {
        gfx_update(mem);
    }
}

void op_mov()
{
    int src = getbyte();
    int dst = getbyte();

    regs[dst] = regs[src];
    info("MOV %%r%d, %%r%d\n", src, dst);
}

void op_add()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] += regs[src];
    info("ADD %%r%d, %%r%d\n", src, dst);
}

void op_sub()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] -= regs[src];
    info("SUB %%r%d, %%r%d\n", src, dst);
}

void op_mul()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] *= regs[src];
    info("MUL %%r%d, %%r%d\n", src, dst);
}

void op_div()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] /= regs[src];
    info("DIV %%r%d, %%r%d\n", src, dst);
}

void op_and()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] &= regs[src];
    info("AND %%r%d, %%r%d\n", src, dst);
}

void op_or()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] |= regs[src];
    info("OR %%r%d, %%r%d\n", src, dst);
}

void op_xor()
{
    int src = getbyte(), dst = getbyte();
    regs[dst] ^= regs[src];
    info("XOR %%r%d, %%r%d\n", src, dst);
}

void op_cmp()
{
    int src = getbyte(), dst = getbyte();
    int8_t res = regs[dst] - regs[src];

    if (res == 0) flags |= ZF;
    else if (res < 0) flags |= SF;
    
    info("CMP %%r%d, %%r%d\n", src, dst);
}

void op_jz()
{
    uint8_t loc = getbyte();
    if (flags & ZF) pc = loc;

    info("JZ $%d\n", loc);
}

void op_jnz()
{
    uint8_t loc = getbyte();
    if (!(flags & ZF)) pc = loc;

    info("JNZ $%d\n", loc);
}

void op_js()
{
    uint8_t loc = getbyte();
    if (flags & SF) pc = loc;
    
    info("JS $%d\n", loc);
}

void op_jns()
{
    uint8_t loc = getbyte();
    if (!(flags & SF)) pc = loc;

    info("JNS $%d\n", loc);
}

void op_jmp()
{
    pc = (getbyte() << 8) | getbyte();
    info("JMP $%d\n", pc);
}
