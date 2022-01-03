    ldr $0x80, %r7
    ldr $0x00, %r0
    ldr $1, %r2

loop:
    ldr $0b1001, %r1    
    str %r1, (%r0)    
    add %r2, %r0
    str %r1, (%r0)    
    jmp $loop

    hlt
