#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch.h>

enum TOKEN
{
    T_IMM,
    T_REG,
    T_LDR,
    T_STR,
    T_MOV,
    T_HLT,
    T_ADD,
    T_SUB,
    T_MUL,
    T_DIV,
    T_AND,
    T_OR,
    T_XOR,
    T_CMP,
    T_JZ,
    T_JNZ,
    T_JS,
    T_JNS,
    T_JMP,
    T_DIRECT,
    T_STRLIT,
    T_LBL,
    T_COMMA,
    T_MEM,
};

struct label
{
    char *name;
    uint16_t pos;
};

struct label *lbls = NULL, *undeflbls = NULL;
unsigned int lblcnt = 0, undeflblcnt = 0;

const char *insts[] =
{
    [T_LDR] = "ldr",
    [T_STR] = "str",
    [T_MOV] = "mov",
    [T_HLT] = "hlt",
    [T_ADD] = "add",
    [T_SUB] = "sub",
    [T_MUL] = "mul",
    [T_DIV] = "sub",
    [T_AND] = "and",
    [T_OR]  = "or",
    [T_XOR] = "xor",
    [T_CMP] = "cmp",
    [T_JZ]  = "jz",
    [T_JNZ] = "jnz",
    [T_JS]  = "js",
    [T_JNS] = "jns",
    [T_JMP] = "jmp"
};

struct token
{
    int type;
    char *sval;
    int ival;
};

FILE *input = NULL, *output = NULL;

int eof()
{
    char c;
    while (isspace(c = fgetc(input)));
    ungetc(c, input);
    return feof(input);
}

struct token next()
{
    char c;
    while (isspace((c = fgetc(input))));

    if (c == '.')
    {
        char str[32];
        size_t strl = 0;
        while ((c = fgetc(input)) != EOF && (isalnum(c) || c == '_')) str[strl++] = c;

        return (struct token) { .type = T_DIRECT, .sval = strdup(str) };
    }
    else if (c == '(')
    {
        struct token t = next();
        t.type = T_MEM;
        fgetc(input); // Expect ')'
        return t;
    }
    else if (c == ',') return (struct token) { .type = T_COMMA };
    else if (c == '$')
    {
        // Immediate
        char str[32];
        size_t strl = 0;

        c = fgetc(input);
        ungetc(c, input);
        printf("char: %c\n", c);
        if (isalpha(c) || c == '_')
        {
            while ((c = fgetc(input)) != EOF && (isalnum(c) || c == '_')) str[strl++] = c;
            str[strl] = 0;
            
            ungetc(c, input);

            printf("lbl: %s\n", str);
            undeflbls = realloc(undeflbls, (undeflblcnt + 1) * sizeof(struct label));
            undeflbls[undeflblcnt++] = (struct label)
            {
                .name = strdup(str),
                .pos  = ftell(output)
            };

            return (struct token) { .type = T_IMM, .ival = 0 };
        }
        else if (c == '\'')
        {
            fgetc(input);
            c = fgetc(input);
            fgetc(input);

            return (struct token) { .type = T_IMM, .ival = c };
        }
        else
        {
            int base = 10;
            if (c == '0')
            {
                fgetc(input);
                c = fgetc(input);
                printf("c: %c\n", c);
                if (c != 'x' && c != 'b' && !isdigit(c))
                    ungetc(c, input);
                else base = c == 'x' ? 16 : c == 'b' ? 2 : 8;
            }

            while ((c = fgetc(input)) != EOF && isxdigit(c)) str[strl++] = c;
            str[strl] = 0;
            
            ungetc(c, input);
            printf("base: %d\n", base);
            uint16_t val = strtol(str, NULL, base);
            return (struct token) { .type = T_IMM, .ival = val };
        }
    }
    else if (c == '%')
    {
        // Register
        fgetc(input); // TODO: expect 'r'
        c = fgetc(input);
        int num = c - '0';
        return (struct token) { .type = T_REG, .ival = num };
    }
    else if (c == '"')
    {
        char str[32];
        size_t strl = 0;

        while ((c = fgetc(input)) != EOF && c != '"') str[strl++] = c;
        fgetc(input);

        return (struct token) { .type = T_STRLIT, .sval = strdup(str) };
    }
    else
    {
        char str[32];
        size_t strl = 0;
        do
        {
            str[strl++] = c;
        }
        while ((c = fgetc(input)) != EOF && isalnum(c));

        str[strl] = 0;
    
        if (c == ':')
        {
            return (struct token) { .type = T_LBL, .sval = strdup(str), .ival = ftell(output) };
        }
        else
        {
            for (int i = 0; i < sizeof(insts) / sizeof(insts[0]); i++)
            {
                if (insts[i] && !strcmp(insts[i], str))
                    return (struct token) { .type = i };
            }
        }
    }
}

void expect(int type)
{
    if (next().type != type)
    {
        // Error
    }
}

void instreg1(int opcode)
{
    fputc(opcode, output);
    struct token reg = next();
    fputc(reg.ival, output);
}

void instreg2(int opcode)
{
    fputc(opcode, output);
    
    struct token src = next();
    fputc(src.ival, output);
    
    expect(T_COMMA);
    struct token dst = next();
    fputc(dst.ival, output);
}

void instjmp(int opcode)
{
    fputc(opcode, output);
    struct token loc = next();

    // TODO: memory/immediate
    fputc(loc.ival >> 8, output);
    fputc(loc.ival & 0xff, output);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: asm <input>\n");
        return -1;
    }

    input = fopen(argv[1], "r");
    if (!input)
    {
        char buf[64];
        snprintf(buf, 64, "asm: %s", argv[1]);
        perror(buf);
        return -1;
    }

    output = fopen("out.bin", "w+");

    while (!eof())
    {
        struct token t = next();
        printf("%d\n", t.type);
        switch (t.type)
        {
            case T_LDR:
            {
                fputc(0, output);
                struct token src = next();
                fseek(output, -1, SEEK_CUR);

                if (src.type == T_IMM)
                    fputc(OP_LDRIMM, output);
                else if (src.type == T_MEM)
                    fputc(OP_LDRMEM, output);

                fputc(src.ival, output);

                expect(T_COMMA);
                struct token dst = next();
                fputc(dst.ival, output);
                break;
            }
            case T_HLT:
            {
                fputc(OP_HLT, output);
                break;
            }
            case T_STR: instreg2(OP_STR); break;
            case T_ADD: instreg2(OP_ADD); break;
            case T_SUB: instreg2(OP_SUB); break;
            case T_MUL: instreg2(OP_MUL); break;
            case T_DIV: instreg2(OP_DIV); break;
            case T_AND: instreg2(OP_AND); break;
            case T_OR:  instreg2(OP_OR);  break;
            case T_XOR: instreg2(OP_XOR); break;
            case T_CMP: instreg2(OP_CMP); break;
            
            case T_JZ:  instjmp(OP_JZ);  break;
            case T_JNZ: instjmp(OP_JNZ); break;
            case T_JS:  instjmp(OP_JS);  break;
            case T_JNS: instjmp(OP_JNS); break;
            case T_JMP: instjmp(OP_JMP); break;
            
            case T_LBL:
            {
                lbls = realloc(lbls, (lblcnt + 1) * sizeof(struct label));
                lbls[lblcnt++] = (struct label)
                {
                    .name = strdup(t.sval),
                    .pos  = ftell(output)
                };
                break;
            }

            case T_DIRECT:
            {
                printf("%s\n", t.sval);
                if (!strcmp(t.sval, "string"))
                {
                    struct token t = next();
                    fprintf(output, "%s", t.sval);
                    fputc(0, output);
                }
                break;
            }
        }
    }

    printf("undef: %d\n", undeflblcnt);
    for (unsigned int i = 0; i < undeflblcnt; i++)
    {
        fseek(output, undeflbls[i].pos, SEEK_SET);
        
        struct label *lbl = NULL;
        for (unsigned int j = 0; j < lblcnt; j++)
        {
            if (!strcmp(lbls[j].name, undeflbls[i].name))
            {
                lbl = &lbls[j];
                break;
            }
        }

        printf("%s: %d\n", lbl->name, lbl->pos);
        //TODO: assert(lbl);
        uint8_t poshi = lbl->pos >> 8;
        uint8_t poslo = lbl->pos & 0xff;
        fwrite(&poshi, 1, 1, output);
        fwrite(&poslo, 1, 1, output);
    }

    fclose(output);
    return 0;
}
