/* https://cirosantilli.com/linux-kernel-module-cheat#lkmc-c
 *
 * This toplevel header includes all the lkmc/ *.h headers.
 */

#ifndef LKMC_H
#define LKMC_H

#if !defined(__ASSEMBLER__)
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LKMC_ASSERT_EQ_DECLARE(bits) \
    void lkmc_assert_eq_ ## bits( \
        uint ## bits ## _t val1, \
        uint ## bits ## _t val2, \
        uint32_t line \
    )
LKMC_ASSERT_EQ_DECLARE(32);
LKMC_ASSERT_EQ_DECLARE(64);
void lkmc_assert_fail(uint32_t line);
void lkmc_assert_memcmp(const void *s1, const void *s2, size_t n, uint32_t line);

#define LKMC_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
/* Standard action to take in case of a file IO error. */
#define LKMC_IO_ERROR(function, path) \
    fprintf(stderr, "error: %s errno = %d, path = %s\n", function, errno, path); \
    exit(EXIT_FAILURE);
#define LKMC_TMP_EXT ".tmp"
/* Temporary per C source file name that our examples can safely create. */
#define LKMC_TMP_FILE __FILE__ LKMC_TMP_EXT
#define LKMC_TMP_FILE_NAMED(name) __FILE__ "__" name LKMC_TMP_EXT
#endif

/* Assert that the given branch instruction is taken. */
#define LKMC_ASSERT(branch_if_pass) \
    branch_if_pass 1f; \
    LKMC_ASSERT_FAIL; \
1: \
;

/* https://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg */
#define LKMC_CONCAT_EVAL(a,b) a ## b
#define LKMC_CONCAT(a,b) LKMC_CONCAT_EVAL(a, b)

#define LKMC_STRINGIFY_DO(x) #x
#define LKMC_STRINGIFY(x) LKMC_STRINGIFY_DO(x)

#define LKMC_GLOBAL(name) \
    .global name; \
    name:

/* Common C definitions. */
#define LKMC_UNUSED(x) (void)x

/* Weak means that if any other file defines it as a non-weak global,
 * that one will take precedence:
 * https://stackoverflow.com/questions/274753/how-to-make-weak-linking-work-with-gcc/54601464#54601464
 */
#define LKMC_WEAK(name) \
    .weak name; \
    name:

#if defined(__x86_64__)
#include <lkmc/x86_64.h>
#elif defined(__arm__)
#include <lkmc/arm.h>
#elif defined(__aarch64__)
#include <lkmc/aarch64.h>
#else
#error
#endif

#include <lkmc/m5ops.h>

#endif
