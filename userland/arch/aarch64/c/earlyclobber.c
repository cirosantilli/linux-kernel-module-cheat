/* An example of using the '&' earlyclobber modifier.
 * https://stackoverflow.com/questions/15819794/when-to-use-earlyclobber-constraint-in-extended-gcc-inline-assembly/54853663#54853663
 * The assertion may fail without it. It actually does fail in GCC 8.2.0 at
 * 34017bcd0bc96a3cf77f6acba4d58350e67c2694 + 1.
 */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t in = 1;
    uint64_t out;
    __asm__ (
        "add %[out], %[in], 1;"
        "add %[out], %[in], 1;"
        : [out] "=&r" (out)
        : [in] "r" (in)
        :
    );
    assert(out == 2);
}
