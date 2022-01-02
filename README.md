# vm
An emulator and assembler for my own 8-bit computer.

## Computer
 - 64 KB of RAM
 - 16 KB of ROM
 - Graphical framebuffer with a resolution of 160x100 with 16 colors

## Assembler

### Syntax
Similar to AT&T syntax:
 - '%' denotes a register e.g. %r0, %r3
 - '$' denotes an immediate, e.g. $100, $0xff, $label
 - Source operand before destination e.g. ```LDR $10, %r0``` loads 10 into r0
 - Value at memory address with parentheses e.g. ```LDR (%r0), %r1``` addresses memory at %r7:%r0 and loads into %r1

### Registers
8 general purpose registers - %r0 - %r7, with the exception that %r7 is used as the upper 8 bits for 16-bit memory addressing (similar to segment registers like %es on x86).

FLAGS register:
 - ZF (zero flag) Bit 0
 - SF (sign flag) Bit 1 

### Instruction Set

 - ```LDR imm, reg``` - Load an immediate into a register
 - ```LDR mem, reg``` - Load a value from memory into a register
 - ```STR reg, mem``` - Store a value in a register into memory
 - ```ADD reg, reg``` - Add src to dst and store in dst
 - ```SUB reg, reg``` - Subtract src from dst and store in dst
 - ```MUL reg, reg``` - Multiply src and dst and store in dst
 - ```DIV reg, reg``` - Divide src by dst and store quotient in dst
 - ```AND reg, reg``` - Bitwise AND src and dst and store in dst
 - ```OR reg, reg``` - Bitwise OR src and dst and store in dst
 - ```XOR reg, reg``` - Bitwise XOR src and <dst and store in dst
 - ```CMP reg, reg``` - Subtract src from dst, without storing result
 - ```JMP imm16``` - Jump to the 16-bit address
 - ```JZ imm16``` - Jump if ZF (zero flag) is set
 - ```JNZ imm16``` - Jump if ZF (zero flag) is not set
 - ```JS imm16``` - Jump if SF (sign flag) is set (negative result)
 - ```JNS imm16``` - Jump if SF (sign flag) is not set (positive result)
 - ```HLT``` - Halt the CPU
 - ```NOP``` - No operation (do nothing)

### Directives
 - ```.string ""``` - Emit a null-terminated string
                                                 
