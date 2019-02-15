/* TODO factor it out to make reusable. */

.global main
main:
    /* Load the vector table. */
    ldr x0, =vector_table
    msr vbar_el1, x0

    /* Do the svc. */
    svc 0

    /* Confirm that svc was called and modified myvar. */
    ldr x0, myvar
    ldr x1, mynewvar
    cmp x0, x1
    beq 1f
    bl common_assert_fail
1:

    /* Go home. */
    ret

common_trap_handler:
    /* Modify myvar as a visible side effect. */
    ldr x0, mynewvar
    ldr x1, =myvar
    str x0, [x1]
    ret

myvar:
    .quad 0x0
mynewvar:
    .quad 0x12346789ABCDEF0

/* .h
 *
 * Adapted from: https://github.com/takeharukato/sample-tsk-sw/blob/ce7973aa5d46c9eedb58309de43df3b09d4f8d8d/hal/aarch64/vector.S
 */

#define AARCH64_EXC_SYNC_SP0     (0x1)
#define AARCH64_EXC_IRQ_SP0      (0x2)
#define AARCH64_EXC_FIQ_SP0      (0x3)
#define AARCH64_EXC_SERR_SP0     (0x4)

#define AARCH64_EXC_SYNC_SPX     (0x11)
#define AARCH64_EXC_IRQ_SPX      (0x12)
#define AARCH64_EXC_FIQ_SPX      (0x13)
#define AARCH64_EXC_SERR_SPX     (0x14)

#define AARCH64_EXC_SYNC_AARCH64 (0x21)
#define AARCH64_EXC_IRQ_AARCH64  (0x22)
#define AARCH64_EXC_FIQ_AARCH64  (0x23)
#define AARCH64_EXC_SERR_AARCH64 (0x24)

#define AARCH64_EXC_SYNC_AARCH32 (0x31)
#define AARCH64_EXC_IRQ_AARCH32  (0x32)
#define AARCH64_EXC_FIQ_AARCH32  (0x33)
#define AARCH64_EXC_SERR_AARCH32 (0x34)

#if !defined(__ASSEMBLER__)
#include <stdint.h>
typedef struct _exception_frame{
    uint64_t exc_type;
    uint64_t exc_esr;
    uint64_t exc_sp;
    uint64_t exc_elr;
    uint64_t exc_spsr;
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
} exception_frame;
void common_trap_handler(exception_frame *exception);
#endif

/* .S */

#define EXC_FRAME_SIZE      (288) /* sizeof(exception_frame)             */
#define EXC_EXC_TYPE_OFFSET (0)   /* offsetof(exception_frame, exc_type) */
#define EXC_EXC_ESR_OFFSET  (8)   /* offsetof(exception_frame, exc_esr)  */
#define EXC_EXC_SP_OFFSET   (16)  /* offsetof(exception_frame, exc_sp)   */
#define EXC_EXC_ELR_OFFSET  (24)  /* offsetof(exception_frame, exc_elr)  */
#define EXC_EXC_SPSR_OFFSET (32)  /* offsetof(exception_frame, exc_spsr) */

#define BUILD_TRAPFRAME(exc_type) \
    stp x29, x30, [sp, -16]!; \
    stp x27, x28, [sp, -16]!; \
    stp x25, x26, [sp, -16]!; \
    stp x23, x24, [sp, -16]!; \
    stp x21, x22, [sp, -16]!; \
    stp x19, x20, [sp, -16]!; \
    stp x17, x18, [sp, -16]!; \
    stp x15, x16, [sp, -16]!; \
    stp x13, x14, [sp, -16]!; \
    stp x11, x12, [sp, -16]!; \
    stp x9, x10, [sp, -16]!; \
    stp x7, x8, [sp, -16]!; \
    stp x5, x6, [sp, -16]!; \
    stp x3, x4, [sp, -16]!; \
    stp x1, x2, [sp, -16]!; \
    mrs x21, spsr_el1; \
    stp x21, x0, [sp, -16]!; \
    mrs x21, elr_el1; \
    stp xzr, x21, [sp, -16]!; \
    mov x21, (exc_type); \
    mrs x22, esr_el1; \
    stp x21, x22, [sp, -16]!

#define STORE_TRAPED_SP \
    mrs x21, sp_el0; \
    str x21, [sp, EXC_EXC_SP_OFFSET]

#define CALL_COMMON_TRAP_HANDLER \
    mov x0, sp; \
    bl common_trap_handler

#define STORE_NESTED_SP \
    mov x21, sp; \
    add x21, x21, EXC_FRAME_SIZE; \
    str x21, [sp, EXC_EXC_SP_OFFSET]

#define RESTORE_TRAPED_SP \
    ldr x21, [sp, EXC_EXC_SP_OFFSET]; \
    msr sp_el0, x21

#define RESTORE_TRAPFRAME \
    add sp, sp, 16; \
    ldp x21, x22, [sp], 16; \
    msr elr_el1, x22; \
    ldp x21, x0, [sp], 16; \
    msr spsr_el1, x21; \
    ldp x1, x2, [sp], 16; \
    ldp x3, x4, [sp], 16; \
    ldp x5, x6, [sp], 16; \
    ldp x7, x8, [sp], 16; \
    ldp x9, x10, [sp], 16; \
    ldp x11, x12, [sp], 16; \
    ldp x13, x14, [sp], 16; \
    ldp x15, x16, [sp], 16; \
    ldp x17, x18, [sp], 16; \
    ldp x19, x20, [sp], 16; \
    ldp x21, x22, [sp], 16; \
    ldp x23, x24, [sp], 16; \
    ldp x25, x26, [sp], 16; \
    ldp x27, x28, [sp], 16; \
    ldp x29, x30, [sp], 16; \
    eret

#define VECTOR_ENTRY(func_name) \
    .align 7; \
    b func_name

#define VECTOR_FUNC_ALIGN .align 2

#define VECTOR_FUNC(func_name, func_id) \
    VECTOR_FUNC_ALIGN; \
func_name:; \
    BUILD_TRAPFRAME(func_id); \
    STORE_TRAPED_SP; \
    CALL_COMMON_TRAP_HANDLER; \
    RESTORE_TRAPED_SP; \
    RESTORE_TRAPFRAME

#define VECTOR_FUNC_NESTED(func_name, func_id) \
    VECTOR_FUNC_ALIGN; \
func_name:; \
    BUILD_TRAPFRAME(func_id); \
    STORE_NESTED_SP; \
    CALL_COMMON_TRAP_HANDLER; \
    RESTORE_TRAPFRAME

    .align 11
    .global vector_table
vector_table:
    VECTOR_ENTRY(_curr_el_sp0_sync)
    VECTOR_ENTRY(_curr_el_sp0_irq)
    VECTOR_ENTRY(_curr_el_sp0_fiq)
    VECTOR_ENTRY(_curr_el_sp0_serror)

    VECTOR_ENTRY(_curr_el_spx_sync)
    VECTOR_ENTRY(_curr_el_spx_irq)
    VECTOR_ENTRY(_curr_el_spx_fiq)
    VECTOR_ENTRY(_curr_el_spx_serror)

    VECTOR_ENTRY(_lower_el_aarch64_sync)
    VECTOR_ENTRY(_lower_el_aarch64_irq)
    VECTOR_ENTRY(_lower_el_aarch64_fiq)
    VECTOR_ENTRY(_lower_el_aarch64_serror)

    VECTOR_ENTRY(_lower_el_aarch32_sync)
    VECTOR_ENTRY(_lower_el_aarch32_irq)
    VECTOR_ENTRY(_lower_el_aarch32_fiq)
    VECTOR_ENTRY(_lower_el_aarch32_serror)

    VECTOR_FUNC(_curr_el_sp0_sync, AARCH64_EXC_SYNC_SP0)
    VECTOR_FUNC(_curr_el_sp0_irq, AARCH64_EXC_IRQ_SP0)
    VECTOR_FUNC(_curr_el_sp0_fiq, AARCH64_EXC_FIQ_SP0)
    VECTOR_FUNC(_curr_el_sp0_serror, AARCH64_EXC_SERR_SP0)

    VECTOR_FUNC_NESTED(_curr_el_spx_sync, AARCH64_EXC_SYNC_SPX)
    VECTOR_FUNC_NESTED(_curr_el_spx_irq, AARCH64_EXC_IRQ_SPX)
    VECTOR_FUNC_NESTED(_curr_el_spx_fiq, AARCH64_EXC_FIQ_SPX)
    VECTOR_FUNC_NESTED(_curr_el_spx_serror, AARCH64_EXC_SERR_SPX)

    VECTOR_FUNC(_lower_el_aarch64_sync, AARCH64_EXC_SYNC_AARCH64)
    VECTOR_FUNC(_lower_el_aarch64_irq, AARCH64_EXC_IRQ_AARCH64)
    VECTOR_FUNC(_lower_el_aarch64_fiq, AARCH64_EXC_FIQ_AARCH64)
    VECTOR_FUNC(_lower_el_aarch64_serror, AARCH64_EXC_SERR_AARCH64)

    VECTOR_FUNC(_lower_el_aarch32_sync, AARCH64_EXC_SYNC_AARCH32)
    VECTOR_FUNC(_lower_el_aarch32_irq, AARCH64_EXC_IRQ_AARCH32)
    VECTOR_FUNC(_lower_el_aarch32_fiq, AARCH64_EXC_FIQ_AARCH32)
    VECTOR_FUNC(_lower_el_aarch32_serror, AARCH64_EXC_SERR_AARCH32)
