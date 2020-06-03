/* A bunch of disassembly/decoding interactive quick-and-dirty test cases. */

.text
.global _start
_start:
    /* MOVZ aliases.
     * gem5 class Movz : RegImmImmOp */
    mov x2, 2
    /* Also Movz class just like 'x' version.. */
    mov w2, 2
    /* Explicit MOVZ with shift. */
    movz x2, 3, lsl 16

    /* MOVN aliases.
     * gem5 class Movz : public RegImmImmOp
     * Negative immediate to see how assembly looks like. */
    mov x2, -2
    mov x2, 0xfffffffffffffffe
    /* Explicit MOVN with shift. */
    movn x2, 1, lsl 16

    /* class AddReg : public DataRegOp
     * The only possible shifts are 0 or 12, every other value
     * is just assembler resolved if possible */
    mov x1, 1
    mov x2, 2
    mov x3, 3
    add x1, x2, x3, lsl 0
    add x1, x2, x3, lsl 12

    /* class AddImm : public DataImmOp TODO confirm AddImm vs AddImmCc */
    add x1, x2, 0
    add x1, x2, 1
    add x1, x2, 1, lsl 12

    /* gem5 LDRWL64_LIT class. */
    ldr w0, =msg
    /* gem5 LDRXL64_LIT class. */
    ldr x0, =msg

    /* gem5 class LDRX64_IMM : public MemoryImm64,
     * implicit 0 immediate omitted. */
    ldr x1, [x0]
    /* gem5 LDRW64_IMM: the W version of the above X. */
    ldr w1, [x0]

    /* gem5 STRW64_IMM */
    str x1, [x0]
    /* gem5 STRW64_IMM */
    str w1, [x0]

    /* Post increment. */
    str x1, [x0], 0
    str x1, [x0], 128

    /* Pre increment. */
    str x1, [x0, 0]!
    str x1, [x0, -128]!

    adr x1, msg
#if 0
    /* Does not exist, addresses are always 64-bit in aarch64. */
    adr w1, msg
#endif
    adrp x1, msg

    /* gem5 class TODO showing non-zero immediate.
     * Assembler automatically emits LDUR
     * because LDR can only represent multiples of 8.
     * https://stackoverflow.com/questions/52894765/ldur-and-stur-in-arm-v8 */
    ldr x1, [x0, 1]

    /* gem5 class LDRX64_REG : public MemoryReg64.
     * The only valid shifts are 0 or 3.
     */
    mov x2, 2
    ldr x1, [x0, x2, lsl 3]

    /* gem5 MemoryImm64, showing . non-zero immediate.
     * Now assembler actually emits LDR because the offset is a multiple of 8. */
    ldr x1, [x0, 16]
    ldr x1, [x0, 256]

    /* Post increment. */
    ldr x1, [x0], 0
    ldr x1, [x0], 128

    /* Pre increment. */
    ldr x1, [x0, 0]!
    ldr x1, [x0, -128]!

    mov x2, 0
    /* gem5 MemoryReg64 without shifts. */
    ldr x1, [x0, x2]

    /* gem5 MemoryEx64
     * x1 (Ws): original compare value and written to from old memory
     * x2 (Wt): new value to write to memory if x1 matches old memory */
    cas x1, x2, [x0]
    cas x1, xzr, [x0]
    cas w1, w2, [x0]
    casb w1, w2, [x0]
    cash w1, w2, [x0]

    mov x2, 0xDEAD
    /* gem5 STXRX64 : public MemoryEx64
     * w1 (Ws): set to 0 success, 1 failure
     * x2 (Wt): new value to write to memory on success */
    stxr w1, x2, [x0]
    /* gem5 STXRW64 */
    stxr w1, w2, [x0]

    /* gem5 MemoryEx64
     * x1 (Ws) : new value that goes to memory
     * x2 (Wt) : written to from old memory */
    swp x1, x2, [x0]

    /* gem5 MemoryEx64
     * x1 (Ws): how much to increment memory by
     * x2 (Wt): old memory value before add written to it */
    mov x1, 0x1234
    str x1, [x0]
    mov x1, 2
    ldadd x1, x2, [x0]
    ldadd w1, w2, [x0]
    /* The smaller versions only have w encodings. */
    ldaddb w1, w2, [x0]
    ldaddh w1, w2, [x0]
    mov x1, 0xDEAD
    stadd x1, [x0]

    /* Was gem5 MemoryEx64, but this is wrong,
     * moved to gem5 MemoryRaw64. */
    ldxr x1, [x0]
    ldxr w1, [x0]
    ldar x1, [x0]

    ldr x0, =msg
    mov x2, 2
    mov x3, 3
    mov x4, 4
    mov x5, 5
    casp x2, x3, x4, x5, [x0]
    mov w2, 2
    mov w3, 3
    mov w4, 4
    mov w5, 5
    casp w2, w3, w4, w5, [x0]

    /* exit */
    mov x0, 0     /* exit status */
    mov x8, 93    /* syscall number */
    svc 0
.data
    /* Align required otherwise this can be 8 byte aligned and CASP can fault. */
    .align 16
msg:
    .quad 0x123456789ABCDEF0
    .skip 1024
