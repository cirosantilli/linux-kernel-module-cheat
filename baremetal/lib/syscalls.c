#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>

#include <lkmc.h>

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

enum {
    UART_FR_RXFE = 0x10,
};

extern char heap_low;
extern char heap_top;

char *heap_end = 0;

void lkmc_baremetal_on_exit_callback(int status, void *arg) {
    (void)arg;
    if (status != 0) {
        printf("lkmc_exit_status_%d\n", status);
    }
}

int _close(int file) {
    LKMC_UNUSED(file);
    return -1;
}

int _fstat(int file, struct stat *st) {
    LKMC_UNUSED(file);
    st->st_mode = S_IFCHR;
    return 0;
}

/* Required by assert. */
int _getpid(void) { return 0; }

/* Required by assert. */
int _kill(pid_t pid, int sig) {
    LKMC_UNUSED(pid);
    exit(128 + sig);
}

int _isatty(int file) {
    LKMC_UNUSED(file);
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    LKMC_UNUSED(file);
    LKMC_UNUSED(ptr);
    LKMC_UNUSED(dir);
    return 0;
}

int _open(const char *name, int flags, int mode) {
    LKMC_UNUSED(name);
    LKMC_UNUSED(flags);
    LKMC_UNUSED(mode);
    return -1;
}

int _read(int file, char *ptr, int len) {
    int todo;
    LKMC_UNUSED(file);
    if (len == 0)
        return 0;
    while (UART_FR(LKMC_UART0_ADDR) & UART_FR_RXFE);
    *ptr++ = UART_DR(LKMC_UART0_ADDR);
    for (todo = 1; todo < len; todo++) {
        if (UART_FR(LKMC_UART0_ADDR) & UART_FR_RXFE) {
            break;
        }
        *ptr++ = UART_DR(LKMC_UART0_ADDR);
    }
    return todo;
}

/* Dummy implementation that just increments an integer. */
_CLOCK_T_ _times_r (struct _reent *r, struct tms *ptms) {
    static long long unsigned t = 0;
    (void)r;
    (void)ptms;
    return t++;
}

caddr_t _sbrk(int incr) {
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
    LKMC_UNUSED(file);
    for (todo = 0; todo < len; todo++) {
        UART_DR(LKMC_UART0_ADDR) = *ptr++;
    }
    return len;
}
