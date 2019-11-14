/* https://cirosantilli.com/linux-kernel-module-cheat#sleep-forever-out */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <unistd.h>

int main(void) {
    puts(__FILE__);
    while (1)
        sleep(0xFFFFFFFF);
}
