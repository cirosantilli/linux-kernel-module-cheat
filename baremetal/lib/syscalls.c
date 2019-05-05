#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <lkmc/m5ops.h>

enum {
    UART_FR_RXFE = 0x10,
};

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

int _close(int file) { return -1; }

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) { return 1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _open(const char *name, int flags, int mode) { return -1; }

int _read(int file, char *ptr, int len) {
    int todo;
    if (len == 0)
        return 0;
    while (UART_FR(UART0_ADDR) & UART_FR_RXFE);
    *ptr++ = UART_DR(UART0_ADDR);
    for (todo = 1; todo < len; todo++) {
        if (UART_FR(UART0_ADDR) & UART_FR_RXFE) {
            break;
        }
        *ptr++ = UART_DR(UART0_ADDR);
    }
    return todo;
}

char *heap_end = 0;
caddr_t _sbrk(int incr) {
    extern char heap_low;
    extern char heap_top;
    char *prev_heap_end;
    if (heap_end == 0) {
        heap_end = &heap_low;
    }
    prev_heap_end = heap_end;
    if (heap_end + incr > &heap_top) {
        /* Heap and stack collision */
        return (caddr_t)0;
    }
    heap_end += incr;
    return (caddr_t)prev_heap_end;
}

int _write(int file, char *ptr, int len) {
    int todo;
    for (todo = 0; todo < len; todo++) {
        UART_DR(UART0_ADDR) = *ptr++;
    }
    return len;
}

/* Only 0 is supported for now, arm semihosting cannot handle other values. */
void _exit(int status) {
#if defined(GEM5)
    LKMC_M5OPS_EXIT;
#else
#if defined(__arm__)
    __asm__ __volatile__ ("mov r0, #0x18; ldr r1, =#0x20026; svc 0x00123456" : : : "r0", "r1");
#elif defined(__aarch64__)
    /* TODO actually use the exit value here, just for fun. */
    __asm__ __volatile__ (
        "mov x1, #0x26\n" \
        "movk x1, #2, lsl #16\n" \
        "str x1, [sp,#0]\n" \
        "mov x0, #0\n" \
        "str x0, [sp,#8]\n" \
        "mov x1, sp\n" \
        "mov w0, #0x18\n" \
        "hlt 0xf000\n"
        :
        :
        : "x0", "x1"
    );
#endif
#endif
}
