# vm
An emulator and assembler for my own 8-bit computer.

## Assembler
 - Similar to AT&T syntax: op src, dst.
 - Registers: %r0 - %r7, %r7 used as "segment" register (upper 8 bits) for 16-bit memory addressing.
 - Flags: ZF - zero flag, SF - sign flag

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
