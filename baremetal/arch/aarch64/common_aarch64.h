#ifndef COMMON_AARCH64_H
#define COMMON_AARCH64_H

#include <inttypes.h>

#define SYSREG_READ(type, name) \
    type sysreg_ ## name ## _read(void) { \
        type name; \
        __asm__ __volatile__("mrs %0, " #name : "=r" (name) : : ); \
        return name; \
    }

#define SYSREG_WRITE(type, name) \
    void sysreg_ ## name ## _write(type name) { \
        __asm__ __volatile__("msr " #name ", %0" : : "r" (name) : ); \
    }

#define SYSREG_READ_WRITE(name, type) \
    SYSREG_READ(name, type) \
    SYSREG_WRITE(name, type)

SYSREG_READ_WRITE(uint32_t, daif)
SYSREG_READ_WRITE(uint32_t, spsel)
SYSREG_READ_WRITE(uint64_t, sp_el1)
SYSREG_READ_WRITE(uint64_t, vbar_el1)

#define SVC(immediate) __asm__ __volatile__("svc " #immediate : : : )

#endif
