end:
    ldr $255, %r0
    ldr $16, %r1
    ldr $0, %r7
    str %r0, (%r1)
    jmp $end
    hlt
