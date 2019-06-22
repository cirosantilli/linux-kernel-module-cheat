# # bswap

    # Little endian <=> big endian.

#include <lkmc.h>

LKMC_PROLOGUE

    mov $0x12345678, %eax
    bswapl %eax
    LKMC_ASSERT_EQ_32(%eax, $0x78563412)

LKMC_EPILOGUE
