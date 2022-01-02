    ldr $str, %r4
    ldr $16, %r1
loop:
    ldr (%r4), %r0
    ldr $0, %r7
    str %r0, (%r1)
    ldr $1, %r2
    add %r2, %r4
    cmp %r0, %r7
    jnz $loop
    hlt

str:
    .string "Hello, world!"
