#include <lkmc.h>

#include <stdio.h>

void atexit_cb(void) {
    puts("atexit");
}

void onexit_cb(int status, void *arg) {
    printf("status %d\n", status);
}

void __attribute__ ((constructor)) premain() {
    printf("premain2()\n");
}

int main(void) {
    atexit(atexit_cb);
    on_exit(onexit_cb, NULL);
    lkmc_assert_fail();
}
