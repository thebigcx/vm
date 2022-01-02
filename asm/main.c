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
    T_LBL,
    T_COMMA,
    T_MEM,
};

const char *insts[] =
{
    [T_LDR] = "ldr",
    [T_STR] = "str",
    [T_MOV] = "mov",
    [T_HLT] = "hlt"
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
    return feof(input);
}

struct token next()
{
    char c;
    while (isspace((c = fgetc(input))));
    printf("%c\n", c);

    if (c == '(')
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
        while ((c = fgetc(input)) != EOF && isxdigit(c)) str[strl++] = c;
        str[strl] = 0;
        
        ungetc(c, input);

        uint8_t val = strtol(str, NULL, 16);
        return (struct token) { .type = T_IMM, .ival = val };
    }
    else if (c == '%')
    {
        // Register
        fgetc(input); // TODO: expect 'r'
        c = fgetc(input);
        int num = c - '0';
        return (struct token) { .type = T_REG, .ival = num };
    }
    else
    {
        char str[32];
        size_t strl = 0;
        do
        {
            str[strl++] = c;
        }
        while ((c = fgetc(input)) != EOF && isalpha(c));

        ungetc(c, input);
        str[strl] = 0;
    
        for (int i = 0; i < sizeof(insts) / sizeof(insts[0]); i++)
        {
            if (insts[i] && !strcmp(insts[i], str))
                return (struct token) { .type = i };
        }

        return (struct token) { .type = T_LBL, .sval = strdup(str) };
    }
}

void expect(int type)
{
    if (next().type != type)
    {
        // Error
    }
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
        switch (t.type)
        {
            case T_LDR:
            {
                struct token src = next();
                expect(T_COMMA);
                struct token dst = next();
                
                if (src.type == T_IMM)
                    fputc(OP_LDRIMM, output);
                else if (src.type == T_MEM)
                    fputc(OP_LDRMEM, output);
                
                fputc(src.ival, output);
                fputc(dst.ival, output);
                break;
            }
            case T_STR:
            {
                struct token src = next();
                expect(T_COMMA);
                struct token dst = next();

                fputc(OP_STR, output);
                fputc(src.ival, output);
                fputc(dst.ival, output);
                break;
            }
            case T_HLT:
            {
                fputc(OP_HLT, output);
                break;
            }
        }
    }

    return 0;
}
